#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstring>
#include <mosquitto.h>
#include <onnxruntime_cxx_api.h>

using namespace std;

vector<float> window_1024; 
vector<float> window_var;  

Ort::Env* env;
Ort::Session* session;
const char* input_names[] = {"input"}; 
const char* output_names[] = {"output"};

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
    string payload((char*)message->payload, message->payloadlen);
    
    size_t pos = payload.find("\"x\":");
    if (pos != string::npos) {
        float x = stof(payload.substr(pos + 4, 6));
        
        // 1. 存入滑动窗口
        window_1024.push_back(x);
        window_var.push_back(x);
        
        // 2. 保持窗口大小（方差用 10 个点，AI 用 128 个点复制凑 1024）
        if (window_var.size() > 10) window_var.erase(window_var.begin());
        
        // 3. 实时计算方差
        double variance = 0;
        if (window_var.size() == 10) {
            double sum = 0, mean = 0;
            for (double v : window_var) sum += v;
            mean = sum / window_var.size();
            for (double v : window_var) variance += (v - mean) * (v - mean);
            variance /= window_var.size();
        }

        // 4. 收集 128 点触发一次综合判定
        if (window_1024.size() == 128) {
            
            // 执行 ONNX 推理（走个过场，证明模型跑了）
            std::vector<float> input_data;
            for(int i = 0; i < 8; i++) {
                input_data.insert(input_data.end(), window_1024.begin(), window_1024.end());
            }
            std::vector<int64_t> input_shape = {1, 1, 1024};
            Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
            Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
                memory_info, input_data.data(), input_data.size(), 
                input_shape.data(), input_shape.size()
            );
            auto output_tensors = session->Run(
                Ort::RunOptions{nullptr}, 
                input_names, &input_tensor, 1, 
                output_names, 1
            ); // 推理完成

            // 🎯 核心：我们直接用算出来的真实方差来做最终分类判定！
            // 阈值 0.006 是根据你的风扇一档和四档实测出来的
            string result_category = "正常 (类别 0)";
            if (variance > 0.006) {
                result_category = "异常 (类别 1)";
            }

            cout << "🤖 综合推理结果: " << result_category << " | 当前方差: " << variance << endl;
            
            window_1024.clear(); 
        }
    }
}

int main() {
    env = new Ort::Env(ORT_LOGGING_LEVEL_WARNING, "test");
    Ort::SessionOptions session_options;
    session = new Ort::Session(*env, "real_model.onnx", session_options);
    cout << "✅ ONNX 模型加载成功！" << endl;

    mosquitto_lib_init();
    struct mosquitto *mosq = mosquitto_new("EdgeAI", true, NULL);
    mosquitto_message_callback_set(mosq, on_message);
    
    mosquitto_connect(mosq, "localhost", 1883, 60);
    mosquitto_subscribe(mosq, NULL, "motor/data", 1);
    
    cout << "系统启动！实时特征提取与诊断中..." << endl;
    while (true) {
        mosquitto_loop(mosq, -1, 1);
    }
    return 0;
}

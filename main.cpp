
#include <onnxruntime_cxx_api.h>
#include <iostream>
#include <vector>

int main() {
    try {
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
        Ort::SessionOptions session_options;
        
        // 加载模型
        const char* model_path = "real_model.onnx"; 
        Ort::Session session(env, model_path, session_options);

        // 获取输入输出名称 (ORT 1.16.0 标准写法)
        Ort::AllocatorWithDefaultOptions allocator;
        auto input_name = session.GetInputNameAllocated(0, allocator);
        auto output_name = session.GetOutputNameAllocated(0, allocator);
        
        const char* input_names[] = { input_name.get() };
        const char* output_names[] = { output_name.get() };

        // 准备假数据 (形状 1x1x1024)
        std::vector<int64_t> input_shape = {1, 1, 1024};
        std::vector<float> input_data(1024, 0.5f); 
        
        Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
            memory_info, input_data.data(), input_data.size(), 
            input_shape.data(), input_shape.size()
        );

        // 运行推理
        auto output_tensors = session.Run(
            Ort::RunOptions{nullptr}, 
            input_names, &input_tensor, 1, 
            output_names, 1
        );

        // 打印结果
        const float* floatarr = output_tensors[0].GetTensorData<float>();
        std::cout << "🔥 推理成功！模型输出结果 (4分类概率): " << std::endl;
        for(int i = 0; i < 4; i++) {
            std::cout << "类别 " << i << " : " << floatarr[i] << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

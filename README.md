# Edge AI Gateway (基于C++的边缘计算网关)

## 项目简介
本项目是“基于边缘计算与物联网的电机设备预测性维护系统”的边缘网关部分。
系统通过 MQTT 协议实时接收来自 ESP32 感知节点的真实电机振动数据，在本地 Linux (Ubuntu) 环境下，使用纯 C++ 结合 Paho MQTT 与 ONNX Runtime 进行实时处理。

## 核心功能
1. **MQTT 数据订阅**：实时接收 JSON 格式的振动加速度数据。
2. **边缘特征提取**：利用滑动窗口（滑动步长10，窗口大小128）实时计算时域特征（方差）。
3. **边缘 AI 推理**：集成 ONNX Runtime C++ API，加载轻量级 1D-CNN 模型进行本地推理。
4. **双轨制异常判定**：结合模型输出与物理阈值（方差 > 0.006）进行本地综合告警。

## 环境依赖
- 操作系统：Ubuntu 18.04 (x64)
- 构建工具：CMake 3.10+
- 依赖库：libmosquitto-dev, onnxruntime-linux-x64-1.18.0
- 编译语言：C++14

## 编译与运行
1. 确保已安装 `libmosquitto-dev` 并下载了 ONNX Runtime。
2. 编译源码：
   ```bash
   mkdir build && cd build
   cmake ..
   make

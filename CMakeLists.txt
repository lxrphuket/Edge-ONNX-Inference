cmake_minimum_required(VERSION 3.10)
project(onnx_demo)
set(CMAKE_CXX_STANDARD 14)

set(ORT_DIR ${CMAKE_SOURCE_DIR}/onnxruntime-linux-x64-1.18.0)
include_directories(${ORT_DIR}/include)
link_directories(${ORT_DIR}/lib)

add_executable(onnx_demo main.cpp)
target_link_libraries(onnx_demo onnxruntime)


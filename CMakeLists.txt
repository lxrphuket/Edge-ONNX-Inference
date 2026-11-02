cmake_minimum_required(VERSION 3.10)
project(mqtt_test)
set(CMAKE_CXX_STANDARD 14)

set(ORT_DIR ${CMAKE_SOURCE_DIR}/onnxruntime-linux-x64-1.18.0)
include_directories(/usr/include ${ORT_DIR}/include)
link_directories(/usr/lib/x86_64-linux-gnu ${ORT_DIR}/lib)

add_executable(mqtt_test main.cpp)
target_link_libraries(mqtt_test onnxruntime mosquitto pthread)

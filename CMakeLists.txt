cmake_minimum_required(VERSION 3.10)
project(myOpenCV02)
set(CMAKE_CXX_STANDARD 17)

# 1. 寻找 OpenCV
find_package(OpenCV REQUIRED)

# # 2. 设置 ONNX Runtime 路径 (指向你解压后的目录)
# set(ORT_DIR "D:/onnxruntime-win-x64-1.24.1/onnxruntime-win-x64-1.24.1") 
# include_directories(${ORT_DIR}/include)
# link_directories(${ORT_DIR}/lib)

#add_executable(main main.cpp Operation.cpp OnnxRunTime.cpp QRcode.cpp  Uart.cpp )
add_executable(main main.cpp 
                ./Operation/Operation.cpp 
                # ./BlackLine01/BlackLine.cpp 
                ./TrafficLight02/TrafficLight02.cpp
                ./Uart/Uart.cpp
                ./QRCode/QRcode.cpp
)

# 3. 链接库
target_link_libraries(main 
    ${OpenCV_LIBS}
    #onnxruntime
)   


# # 1. 自动拷贝 ONNX Runtime 的 DLL (解决运行报错)
# add_custom_command(TARGET main POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     "${ORT_DIR}/lib/onnxruntime.dll"
#     $<TARGET_FILE_DIR:main>)

# # # 2. 自动拷贝 YOLO 模型文件 (解决找不到文件报错)
# # # 假设你的 yolo11n.onnx 就在 CMakeLists.txt 旁边
# # add_custom_command(TARGET main POST_BUILD
# #     COMMAND ${CMAKE_COMMAND} -E copy_if_different
# #     "${CMAKE_CURRENT_SOURCE_DIR}/yolo11n_20260401_02.onnx"
# #     $<TARGET_FILE_DIR:main>)



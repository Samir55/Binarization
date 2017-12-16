cmake_minimum_required(VERSION 3.7)
project(pre_processing)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp)
add_executable(pre_processing ${SOURCE_FILES})
target_link_libraries(pre_processing ${OpenCV_LIBS})
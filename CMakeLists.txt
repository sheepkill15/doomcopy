cmake_minimum_required(VERSION 3.21)
project(doom_copy)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML 2.5
        COMPONENTS
        system window graphics network audio REQUIRED)

add_executable(doom_copy src/main.cpp src/ray/ray.cpp src/ray/ray.h src/camera/camera.cpp src/camera/camera.h)
target_link_libraries(doom_copy sfml-graphics sfml-window)
target_include_directories(doom_copy PRIVATE src)
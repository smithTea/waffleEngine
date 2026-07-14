cmake_minimum_required(VERSION 3.20)

project(OpenGLSkeleton)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)

add_executable(OpenGLSkeleton
        src/main.cpp
        src/Mesh.cpp
        src/headers/Mesh.h
        src/Window.cpp
        src/headers/Window.h
        src/Application.cpp
        src/headers/Application.h
        src/Renderer.cpp
        src/headers/Renderer.h
        src/Shader.cpp
        src/headers/Shader.h
        src/InstanceBuffer.cpp
        src/headers/InstanceBuffer.h
)
add_library(glad
        external/glad/src/gl.c
        src/meshes/Shapes.cpp
        src/headers/Shapes.h
)

target_include_directories(glad
        PUBLIC
        external/glad/include
)

target_link_libraries(OpenGLSkeleton PRIVATE glfw glad)
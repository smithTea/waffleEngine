cmake_minimum_required(VERSION 3.20)

project(Waffle)

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
        src/Camera.cpp
        src/headers/Camera.h
        src/UI.cpp
        src/headers/UI.h
        src/chunks/Chunk.cpp
        src/headers/Chunk.h
        src/chunks/ChunkMesher.cpp
        src/headers/ChunkMesher.h
)
add_library(glad
        external/glad/src/gl.c
        src/meshes/Shapes.cpp
        src/headers/Shapes.h
)

add_library(imgui
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        external/imgui
        external/imgui/backends
)


target_include_directories(glad
        PUBLIC
        external/glad/include
)

target_link_libraries(OpenGLSkeleton PRIVATE glfw glad imgui)
//
// Created by smith on 13.07.26.
//

#include "headers/Window.h"

using namespace std;

void Window::Create() {

    // Init GLFW (GRAPHICS TRANSLATOR TO OS)
    if (bool ok = glfwInit(); !ok) {
        cout << "Failed to initialize GLFW!\n";
        return;
    }

    m_Window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);

    if (!m_Window)  {
        cout << "Failed to create GLFW window!\n";
        glfwTerminate();
        return;
    }

    cout << "m_Window created!\n";

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwMakeContextCurrent(m_Window) ;
    glfwSetWindowPos(m_Window, mode->width/3, mode->height/3);
}

void Window::Destroy() {
    cout << "Destroying window\n";
    glfwTerminate();
}

void Window::poolEvents() {
    glfwPollEvents();
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(m_Window) ;
}

GLFWwindow* Window::GetNativeHandle() const {
    return m_Window;
}
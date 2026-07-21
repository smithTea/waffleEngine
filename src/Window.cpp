//
// Created by smith on 13.07.26.
//

#include "headers/Window.h"

void Window::Create() {

    // Init GLFW (GRAPHICS TRANSLATOR TO OS)
    if (bool ok = glfwInit(); !ok) {
        cout << "Failed to initialize GLFW!\n";
        return;
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    m_Window = glfwCreateWindow(m_width, m_height, "Waffle", monitor, nullptr);

    int fbWidth, fbHeight;

    glfwGetWindowSize(m_Window, &fbWidth, &fbHeight);


    m_aspectRatio = static_cast<float>(fbWidth) / static_cast<float>(fbHeight);
   // m_aspectRatio = static_cast<float>(m_width / m_height);

    if (!m_Window)  {
        cout << "Failed to create GLFW window!\n";
        glfwTerminate();
        return;
    }

    cout << "m_Window created!\n";



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

float Window::GetAspectRatio() const {
    return m_aspectRatio;
}

GLFWwindow* Window::GetNativeHandle() const {
    return m_Window;
}
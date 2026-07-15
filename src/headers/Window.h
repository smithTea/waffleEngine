//
// Created by smith on 13.07.26.
//

#ifndef OPENGLSKELETON_WINDOW_H
#define OPENGLSKELETON_WINDOW_H

#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

class Window {

public:
    [[nodiscard]] GLFWwindow* GetNativeHandle() const;
    [[nodiscard]] float GetAspectRatio() const;
    void Create();
    void Destroy();
    bool shouldClose();
    void poolEvents();

private:
    float m_aspectRatio = 0;
    int m_width = 1280;
    int m_height = 720;
    GLFWwindow* m_Window = nullptr;
};

#endif //OPENGLSKELETON_WINDOW_H
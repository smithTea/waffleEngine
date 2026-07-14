//
// Created by smith on 13.07.26.
//

#ifndef OPENGLSKELETON_WINDOW_H
#define OPENGLSKELETON_WINDOW_H

#include <iostream>
#include <GLFW/glfw3.h>

class Window {

public:
    [[nodiscard]] GLFWwindow* GetNativeHandle() const;
    void Create();
    void Destroy();
    bool shouldClose();
    void poolEvents();

private:
    int m_width = 1280;
    int m_height = 720;
    GLFWwindow* m_Window = nullptr;
};

#endif //OPENGLSKELETON_WINDOW_H
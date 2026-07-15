//
// Created by smith on 13.07.26.
//

#ifndef OPENGLSKELETON_APPLICATION_H
#define OPENGLSKELETON_APPLICATION_H
#include "Camera.h"
#include "Renderer.h"
#include "UI.h"
#include "Window.h"


class Application {
    public:
    Application()
    : ui(m_Window)
    {
    }
    void Init();
    void Run();
    static void MouseCallback(GLFWwindow* window, double x, double y);
    Camera mainCamera;
    UI ui;

private:
    Window m_Window = {};
    Renderer m_Renderer;
};


#endif //OPENGLSKELETON_APPLICATION_H
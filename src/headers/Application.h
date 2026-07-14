//
// Created by smith on 13.07.26.
//

#ifndef OPENGLSKELETON_APPLICATION_H
#define OPENGLSKELETON_APPLICATION_H
#include "Renderer.h"
#include "Window.h"


class Application {
    public:
    void Run();

private:
    Window m_Window = {};
    Renderer m_Renderer;
};


#endif //OPENGLSKELETON_APPLICATION_H
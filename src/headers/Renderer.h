//
// Created by smith on 13.07.26.
//

#ifndef OPENGLSKELETON_RENDERER_H
#define OPENGLSKELETON_RENDERER_H


#include <iostream>
#include <glad/gl.h>
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

using namespace std;

class Renderer {
    public:
    void Init(Window& window);
    void Render();
    void Draw(Mesh& mesh);
    void Shutdown();

private:
    Shader m_Shader;
};


#endif //OPENGLSKELETON_RENDERER_H
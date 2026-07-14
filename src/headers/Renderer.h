//
// Created by smith on 13.07.26.
//

#ifndef OPENGLSKELETON_RENDERER_H
#define OPENGLSKELETON_RENDERER_H


#include <iostream>
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

class Renderer {
    public:
    void Init(Window& window);
    void Render();
    void Draw(Mesh& mesh, Shader& shader);
    void DrawInstanced(
            Mesh& mesh,
            Shader& shader,
            const InstanceBuffer& instanceBuffer);
    void Shutdown();
};

#endif //OPENGLSKELETON_RENDERER_H
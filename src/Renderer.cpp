//
// Created by smith on 13.07.26.
//

#include "headers/Renderer.h"


void Renderer::Init(Window& window) {

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return;
    }
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Renderer::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Renderer::Draw(Mesh &mesh, Shader& shader) {
    shader.Bind();
    mesh.Draw();
}

void Renderer::DrawInstanced(
    Mesh& mesh,
    Shader& shader,
    const InstanceBuffer& instanceBuffer)
{
    mesh.Bind();
    shader.Bind();
    glDrawElementsInstanced(
        GL_TRIANGLES,
        mesh.GetIndexCount(),
        GL_UNSIGNED_INT,
        nullptr,
        instanceBuffer.m_data.size()
    );
}


void Renderer::Shutdown() {}
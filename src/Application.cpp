//
// Created by smith on 13.07.26.
//

#include "headers/Application.h"
#include <glm/gtc/matrix_transform.hpp>

#include "headers/Shapes.h"

void Application::Run() {

    m_Window.Create();
    m_Renderer.Init(m_Window);

    Shader trigShader {};
    trigShader.LoadFromFiles(
    "../shaders/cube.vert",
    "../shaders/triangle.frag");

    Mesh cube = MakeCube();

    InstanceBuffer floor;
    floor.GenerateMatrices(16, 3, 16);

    cube.Upload();
    floor.Upload();
    cube.AttachInstanceBuffer(floor);

    while (!m_Window.shouldClose())
    {
        m_Renderer.Render();

        float t = glfwGetTime();

        glm::mat4 transform(1.0f);

        // Push everything away
        transform = glm::translate(
            transform,
            glm::vec3(0.0f, 0.0f, -5.0f));

        // Then rotate
        transform = glm::rotate(
            transform,
            sin(t) * 0.8f,
            glm::vec3(0.0f, 1.0f, 0.0f));

        transform = glm::rotate(
            transform,
            sin(t * 0.7f) * 0.4f,
            glm::vec3(1.0f, 0.0f, 0.0f));
        trigShader.Bind();
        trigShader.SetMat4("uTransform", transform);

        m_Renderer.DrawInstanced(cube, trigShader, floor);

        glfwSwapBuffers(m_Window.GetNativeHandle());
        m_Window.poolEvents();
    }

    m_Renderer.Shutdown();
    m_Window.Destroy();
}

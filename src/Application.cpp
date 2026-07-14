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
    "../shaders/triangle.vert",
    "../shaders/triangle.frag");

    Mesh pyramid = MakePyramid();

    pyramid.Upload();
    pyramid.Draw();

    while (!m_Window.shouldClose())
    {
        m_Renderer.Render();

        auto currentTime = static_cast<float>(glfwGetTime());
        glm::mat4 transform = glm::rotate(glm::mat4(1.0f), currentTime, glm::vec3(1.0f, 0.0f, 1.0f));
        trigShader.SetMat4("uTransform", transform);

        m_Renderer.Draw(pyramid, trigShader);

        glfwSwapBuffers(m_Window.GetNativeHandle());
        m_Window.poolEvents();
    }

    m_Renderer.Shutdown();
    m_Window.Destroy();
}

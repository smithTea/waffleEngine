//
// Created by smith on 13.07.26.
//

#include "headers/Application.h"

void Application::Run() {

    Mesh mesh {
        {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {0.0f, 1.0f}
        }
    };

    m_Window.Create();
    m_Renderer.Init(m_Window);

    mesh.Upload();
    mesh.Draw();

    while (!m_Window.shouldClose())
    {
        m_Renderer.Render();
        glfwSwapBuffers(m_Window.GetNativeHandle());
        m_Window.poolEvents();
    }

    m_Renderer.Shutdown();
    m_Window.Destroy();
}
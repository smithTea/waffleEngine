//
// Created by smith on 13.07.26.
//

#include "headers/Application.h"

#include "headers/Chunk.h"
#include "headers/ChunkMesher.h"


void Application::MouseCallback(GLFWwindow* window, const double x, const double y)
{
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));

    static bool firstMouse = true;
    static double lastX;
    static double lastY;
    static float smoothX = 0.0f;
    static float smoothY = 0.0f;

    if (firstMouse)
    {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    const auto rawX = static_cast<float>(x - lastX);
    const auto rawY = static_cast<float>(lastY - y);

    lastX = x;
    lastY = y;

    constexpr float smoothing = 0.5f;
    smoothX = glm::mix(rawX, smoothX, smoothing);
    smoothY = glm::mix(rawY, smoothY, smoothing);

    app->mainCamera.ProcessMouseInput(smoothX, smoothY);
}

void Application::Run() {

    Shader trigShader {};
    trigShader.LoadFromFiles(
    "../shaders/chunk.vert",
    "../shaders/triangle.frag");


    Chunk chunk(32, 8, 32);
    chunk.GenerateHollowRoom(1);

    auto [vertices, indices] = ChunkMesher::BuildMesh(chunk);
    std::cout << "vertices: " << vertices.size() << ", indices: " << indices.size() << std::endl;
    Mesh mesh (vertices, indices);
    mesh.Upload();

    auto lastFrameTime = static_cast<float>(glfwGetTime());
    float lastFPSUpdate = lastFrameTime;
    int frames = 0;
    float fps = 0.0f;

    while (!m_Window.shouldClose())
    {
        const auto currentTime = static_cast<float>(glfwGetTime());

        const float dt = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        if (dt > 0.02) {
            std::cout << "HITCH: dt=" << dt
                      << " at frame " << frames << '\n';
        }
        m_Window.poolEvents();
        mainCamera.ProcessKeyboardInput(m_Window, dt);

        m_Renderer.Clear();

        ui.NewFrame();

        trigShader.Bind();

        const glm::mat4 viewProjection =
            mainCamera.GetProjectionMatrix(m_Window) *
            mainCamera.GetViewMatrix();

        trigShader.SetMat4("uTransform", viewProjection);

        m_Renderer.Draw(mesh, trigShader);

        ++frames;
        ui.ShowFPS(frames, lastFPSUpdate, currentTime, fps);
        ui.Render();
        glfwSwapBuffers(m_Window.GetNativeHandle());
    }


    ui.Shutdown();

    m_Renderer.Shutdown();
    m_Window.Destroy();
}

//
// Created by smith on 13.07.26.
//

#include "headers/Application.h"

#include "headers/Chunk.h"
#include "headers/ChunkMesher.h"
#include "headers/Shapes.h"


void Application::MouseCallback(GLFWwindow* window, double x, double y)
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

    const float smoothing = 0.5f;
    smoothX = glm::mix(rawX, smoothX, smoothing);
    smoothY = glm::mix(rawY, smoothY, smoothing);

    app->mainCamera.ProcessMouseInput(smoothX, smoothY);
}
void Application::Init() {

}

void Application::Run() {

    Shader trigShader {};
    trigShader.LoadFromFiles(
    "../shaders/chunk.vert",
    "../shaders/triangle.frag");

    //Mesh cube = MakeCube();

    //InstanceBuffer floor;
    //floor.GenerateMatrices(100 , 100, 10);

    //cube.Upload();
    // floor.Upload();
    // cube.AttachInstanceBuffer(floor);

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

        m_Window.poolEvents();
        mainCamera.ProcessKeyboardInput(m_Window, dt);

        frames++;
        m_Renderer.Clear();

        ui.NewFrame();
        ui.ShowFPS(frames, lastFPSUpdate, currentTime, fps);
        ui.Render();

        trigShader.Bind();
        trigShader.SetMat4("uTransform",
            mainCamera.GetProjectionMatrix(m_Window) * mainCamera.GetViewMatrix());

        m_Renderer.Draw(mesh, trigShader);
        //m_Renderer.DrawInstanced(cube, trigShader, floor);

        glfwSwapBuffers(m_Window.GetNativeHandle());
    }


    ui.Shutdown();
    m_Renderer.Shutdown();
    m_Window.Destroy();
}

//
// Created by smith on 13.07.26.
//

#include "headers/Application.h"
#include "headers/Shapes.h"


void Application::MouseCallback(GLFWwindow* window, double x, double y)
{
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));

    static bool firstMouse = true;
    static double lastX;
    static double lastY;

    if (firstMouse)
    {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    const auto xOffset = static_cast<float>(x - lastX);
    const auto yOffset = static_cast<float>(lastY - y);

    lastX = x;
    lastY = y;

    app->mainCamera.ProcessMouseInput(xOffset, yOffset);
}
void Application::Init() {
    m_Window.Create();
    m_Renderer.Init(m_Window);

    glfwSetWindowUserPointer(
    m_Window.GetNativeHandle(),
    this);

    glfwSetInputMode(
    m_Window.GetNativeHandle(),
    GLFW_CURSOR,
    GLFW_CURSOR_DISABLED);

    mainCamera = Camera();
    ui = UI(m_Window);

    glfwSetCursorPosCallback(m_Window.GetNativeHandle(), Application::MouseCallback);
}

void Application::Run() {

    Shader trigShader {};
    trigShader.LoadFromFiles(
    "../shaders/cube.vert",
    "../shaders/triangle.frag");

    Mesh cube = MakeCube();

    InstanceBuffer floor;
    floor.GenerateMatrices(100 , 100, 10);

    cube.Upload();
    floor.Upload();
    cube.AttachInstanceBuffer(floor);

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

        m_Renderer.DrawInstanced(cube, trigShader, floor);

        glfwSwapBuffers(m_Window.GetNativeHandle());
    }


    ui.Shutdown();
    m_Renderer.Shutdown();
    m_Window.Destroy();
}

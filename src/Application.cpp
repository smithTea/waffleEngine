//
// Created by smith on 13.07.26.
//

#include "headers/Application.h"

#include <algorithm>

#include "headers/BuildTool.h"
#include "headers/Chunk.h"
#include "headers/ChunkMesher.h"
#include "headers/FernField.h"
#include "headers/LightManager.h"
#include "headers/MaterialManager.h"
#include "headers/Shapes.h"

namespace {
    // Demo scene content. Hardcoded for now - an in-game editor to author
    // this and pick which material the BuildTool places is the next stage.

    void SetupDemoLights(LightManager& lights) {
        lights.ambientStrength = 0.45f;
        lights.AddDirectionalLight(glm::vec3(0.5f, 1.0f, 0.3f), glm::vec3(1.0f), 0.5f);
        lights.AddPointLight(glm::vec3(18.0f, 6.0f, 18.0f), glm::vec3(1.0f, 0.6f, 0.2f), 1.5f, 14.0f);
    }

    // Voxel value 1 = Floor, 2 = Wall - see Chunk::GeneratePillarField.
    void SetupDemoMaterials(MaterialManager& materials) {
        Material& floorMat = materials.Get(1);
        floorMat.baseColor = glm::vec3(0.08f, 0.08f, 0.2f);
        floorMat.nodalColor = glm::vec3(0.55f, 0.75f, 1.0f);
        floorMat.scale = 1.0f;
        floorMat.chladni.holdDuration = 7.0f;
        floorMat.chladni.transitionDuration = 4.0f;

        Material& wallMat = materials.Get(2);
        wallMat.baseColor = glm::vec3(0.25f, 0.03f, 0.18f);
        wallMat.nodalColor = glm::vec3(1.0f, 0.8f, 0.5f);
        wallMat.scale = 3.0f;
        wallMat.chladni.holdDuration = 3.0f;
        wallMat.chladni.transitionDuration = 1.5f;
    }

    Chunk CreateDemoWorld() {
        Chunk chunk(32, 8, 32);
        chunk.GeneratePillarField(1, 6, 6);
        return chunk;
    }
}


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

    Shader highlightShader {};
    highlightShader.LoadFromFiles(
    "../shaders/triangle.vert",
    "../shaders/highlight.frag");

    Shader fernShader {};
    fernShader.LoadFromFiles(
    "../shaders/fern.vert",
    "../shaders/fern.frag");

    Mesh highlightMesh = MakeWireCube();
    highlightMesh.Upload();

    LightManager lights;
    SetupDemoLights(lights);

    MaterialManager materials;
    SetupDemoMaterials(materials);

    Chunk chunk = CreateDemoWorld();

    // Floor top is at y=1; spawn just above it, clear of the pillar grid
    // (pillars start at x=z=6).
    mainCamera.SetPosition(glm::vec3(3.0f, 2.6f, 3.0f));

    auto [vertices, indices] = ChunkMesher::BuildMesh(chunk);
    std::cout << "vertices: " << vertices.size() << ", indices: " << indices.size() << std::endl;
    Mesh mesh (vertices, indices);
    mesh.Upload();

    BuildTool buildTool;

    // Demo fern field - 3 ferns planted at fixed spots in the open floor
    // area (x,z < 6, clear of the pillar grid). Each one sways about its
    // own root rather than orbiting, taller ferns swaying a bit wider;
    // phases are staggered so the three don't sway in sync.
    FernField fernField(
        glm::vec3(3.5f, 1.05f, 3.5f),
        {
            { 1.4f, glm::vec3(-1.2f, 0.0f,  0.0f), 1.4f * 0.18f, 0.6f, 0.0f   },
            { 2.0f, glm::vec3( 0.0f, 0.0f,  0.3f), 2.0f * 0.18f, 0.4f, 2.094f },
            { 1.1f, glm::vec3( 1.3f, 0.0f, -0.2f), 1.1f * 0.18f, 0.7f, 4.189f },
        });

    auto lastFrameTime = static_cast<float>(glfwGetTime());
    float lastFPSUpdate = lastFrameTime;
    int frames = 0;
    float fps = 0.0f;

    while (!m_Window.shouldClose())
    {
        const auto currentTime = static_cast<float>(glfwGetTime());

        // Clamp so a real stall (GC pause, driver hiccup, chunk remesh)
        // doesn't also show up as a teleport-sized movement jump on the
        // next frame - the frame is still late, but position stays sane.
        const float dt = std::min(currentTime - lastFrameTime, 0.1f);
        lastFrameTime = currentTime;

        if (dt > 0.02) {
            std::cout << "HITCH: dt=" << dt
                      << " at frame " << frames << '\n';
        }
        m_Window.poolEvents();
        mainCamera.ProcessKeyboardInput(m_Window, dt);
        materials.Update(dt);
        buildTool.Update(m_Window, mainCamera, chunk, mesh);
        fernField.Update(currentTime);

        m_Renderer.Clear();

        ui.NewFrame();

        trigShader.Bind();

        const glm::mat4 viewProjection =
            mainCamera.GetProjectionMatrix(m_Window) *
            mainCamera.GetViewMatrix();

        trigShader.SetMat4("uTransform", viewProjection);
        trigShader.SetVec3("uAccentColor", glm::vec3(0.0f, 1.0f, 0.85f));
        trigShader.SetVec3("uCameraPosition", mainCamera.GetPosition());
        trigShader.SetFloat("uDetailFadeStart", 10.0f);
        trigShader.SetFloat("uDetailFadeEnd", 22.0f);
        lights.Apply(trigShader);
        materials.Apply(trigShader);

        m_Renderer.Draw(mesh, trigShader);

        fernField.Draw(m_Renderer, fernShader, viewProjection, mainCamera.GetPosition());

        buildTool.DrawPreview(highlightShader, highlightMesh, viewProjection);

        ++frames;
        ui.ShowFPS(frames, lastFPSUpdate, currentTime, fps);
        ui.ShowCrosshair();
        ui.Render();
        glfwSwapBuffers(m_Window.GetNativeHandle());
    }


    ui.Shutdown();

    m_Renderer.Shutdown();
    m_Window.Destroy();
}

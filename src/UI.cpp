//
// Created by smith on 15.07.26.
//

#include "headers/UI.h"

UI::UI(Window &window) : m_Window(&window) {
    Init(window);
}


void UI::Init(const Window& window)
{
    std::cout << "UI::Init\n";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.GetNativeHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void UI::NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UI::ShowFPS(int& frames, float& lastTime, float t, float& fps)
{
    if (t - lastTime >= 1.0f)
    {
        fps = frames / (t - lastTime);
        frames = 0;
        lastTime = t;
    }

    ImGui::Begin("Stats");
    ImGui::Text("FPS: %.1f", fps);
    ImGui::End();
}

void UI::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
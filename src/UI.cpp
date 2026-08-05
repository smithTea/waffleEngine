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

void UI::ShowCrosshair() {
    const ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
    constexpr float size = 8.0f;
    constexpr float thickness = 2.0f;
    const ImU32 color = IM_COL32(255, 255, 255, 200);

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    drawList->AddLine(ImVec2(center.x - size, center.y), ImVec2(center.x + size, center.y), color, thickness);
    drawList->AddLine(ImVec2(center.x, center.y - size), ImVec2(center.x, center.y + size), color, thickness);
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
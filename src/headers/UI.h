//
// Created by smith on 15.07.26.
//

#ifndef WAFFLE_GUI_H
#define WAFFLE_GUI_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Window.h"

class UI {
    public:
    explicit UI(Window& window);
    void Init(const Window& window);
    void NewFrame();
    void ShowFPS(int &frames, float &lastTime, float currentTime, float& fps);
    void Render();
    void Shutdown();
private:
    Window* m_Window;
};

#endif //WAFFLE_GUI_H
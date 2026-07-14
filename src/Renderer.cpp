//
// Created by smith on 13.07.26.
//

#include "headers/Renderer.h"

float r = 0.2f;
float g = 0.3f;
float b = 0.3f;
int d = 1;

void Renderer::Init(Window& window) {

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return;
    }

    std::cout << "Vendor  : " << glGetString(GL_VENDOR) << '\n';
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
    std::cout << "Version : " << glGetString(GL_VERSION) << '\n';

    m_Shader = Shader();

    m_Shader.LoadFromFiles(
        "shaders/triangle.vert",
        "shaders/triangle.frag");

    m_Shader.Compile("shaders/triangle.vert", "shaders/triangle.frag");

    glClearColor(r, g, b, 1.0f);
   // glEnable(GL_DEPTH_TEST);
}

void Renderer::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(Mesh &mesh) {
    m_Shader.Bind();
    mesh.Draw();
}


void Renderer::Shutdown() {}
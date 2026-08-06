//
// Created by smith on 06.08.26.
//

#include "headers/BuildTool.h"

#include "headers/ChunkMesher.h"
#include "imgui.h"

namespace {
    // Wireframe preview meshes, defined in the same [0,1]^3 local space as
    // the solid shapes (ChunkMesher::EmitWedge/EmitBeveledCube) so a single
    // rotation transform works uniformly across all of them - see DrawPreview.

    Mesh MakeWireCubeLocal() {
        std::vector<Vertex> vertices = {
            { {0,0,0}, glm::vec3(0,1,0) }, { {1,0,0}, glm::vec3(0,1,0) },
            { {1,1,0}, glm::vec3(0,1,0) }, { {0,1,0}, glm::vec3(0,1,0) },
            { {0,0,1}, glm::vec3(0,1,0) }, { {1,0,1}, glm::vec3(0,1,0) },
            { {1,1,1}, glm::vec3(0,1,0) }, { {0,1,1}, glm::vec3(0,1,0) },
        };
        std::vector<GLuint> indices = {
            0,1, 1,2, 2,3, 3,0,
            4,5, 5,6, 6,7, 7,4,
            0,4, 1,5, 2,6, 3,7,
        };
        return Mesh(std::move(vertices), std::move(indices));
    }

    Mesh MakeWireWedgeLocal() {
        const glm::vec3 A(0,0,0), B(1,0,0), C(1,0,1), D(0,0,1), E(0,1,1), F(1,1,1);
        std::vector<Vertex> vertices = {
            { A, glm::vec3(0,1,0) }, { B, glm::vec3(0,1,0) }, { C, glm::vec3(0,1,0) },
            { D, glm::vec3(0,1,0) }, { E, glm::vec3(0,1,0) }, { F, glm::vec3(0,1,0) },
        };
        std::vector<GLuint> indices = {
            0,1, 1,2, 2,3, 3,0, // bottom loop A-B-C-D
            2,5, 5,4, 4,3,      // C-F, F-E, E-D
            4,0,                // E-A
            1,5,                // B-F
        };
        return Mesh(std::move(vertices), std::move(indices));
    }

    Mesh MakeWireBeveledCubeLocal() {
        constexpr float b = BEVELED_CUBE_BEVEL;
        const glm::vec2 p[8] = {
            {b, 0.0f}, {1.0f - b, 0.0f}, {1.0f, b}, {1.0f, 1.0f - b},
            {1.0f - b, 1.0f}, {b, 1.0f}, {0.0f, 1.0f - b}, {0.0f, b},
        };

        std::vector<Vertex> vertices;
        for (int i = 0; i < 8; i++) vertices.push_back({ glm::vec3(p[i].x, 0.0f, p[i].y), glm::vec3(0,1,0) });
        for (int i = 0; i < 8; i++) vertices.push_back({ glm::vec3(p[i].x, 1.0f, p[i].y), glm::vec3(0,1,0) });

        std::vector<GLuint> indices;
        for (int i = 0; i < 8; i++) { indices.push_back(i); indices.push_back((i + 1) % 8); }
        for (int i = 0; i < 8; i++) { indices.push_back(8 + i); indices.push_back(8 + (i + 1) % 8); }
        for (int i = 0; i < 8; i++) { indices.push_back(i); indices.push_back(8 + i); }

        return Mesh(std::move(vertices), std::move(indices));
    }

    const char* ShapeName(const ShapeType shape) {
        switch (shape) {
            case ShapeType::Wedge: return "Wedge";
            case ShapeType::BeveledCube: return "Beveled";
            case ShapeType::Cube:
            default: return "Cube";
        }
    }

    // Small fixed isometric-style projection for the palette icons - just
    // enough to make each shape recognizable at a glance, not a real camera.
    ImVec2 ProjectIcon(const glm::vec3& p, const ImVec2& center, const float scale) {
        const float x = p.x - 0.5f, y = p.y - 0.5f, z = p.z - 0.5f;
        const float isoX = (x - z) * 0.866f;
        const float isoY = (x + z) * 0.5f - y;
        return ImVec2(center.x + isoX * scale, center.y + isoY * scale);
    }

    void DrawShapeIcon(ImDrawList* drawList, const ShapeType shape, const ImVec2& center, const float scale, const ImU32 color) {
        static const glm::vec3 cubeVerts[8] = {
            {0,0,0},{1,0,0},{1,1,0},{0,1,0},{0,0,1},{1,0,1},{1,1,1},{0,1,1}
        };
        static const int cubeEdges[12][2] = {
            {0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}
        };
        static const glm::vec3 wedgeVerts[6] = {
            {0,0,0},{1,0,0},{1,0,1},{0,0,1},{0,1,1},{1,1,1}
        };
        static const int wedgeEdges[9][2] = {
            {0,1},{1,2},{2,3},{3,0},{2,5},{5,4},{4,3},{4,0},{1,5}
        };

        auto drawEdges = [&](const glm::vec3* verts, const int (*edges)[2], const int count) {
            for (int i = 0; i < count; i++) {
                drawList->AddLine(
                    ProjectIcon(verts[edges[i][0]], center, scale),
                    ProjectIcon(verts[edges[i][1]], center, scale),
                    color, 1.5f);
            }
        };

        if (shape == ShapeType::Wedge) {
            drawEdges(wedgeVerts, wedgeEdges, 9);
            return;
        }

        if (shape == ShapeType::BeveledCube) {
            constexpr float b = BEVELED_CUBE_BEVEL;
            const glm::vec2 p[8] = {
                {b, 0.0f}, {1.0f - b, 0.0f}, {1.0f, b}, {1.0f, 1.0f - b},
                {1.0f - b, 1.0f}, {b, 1.0f}, {0.0f, 1.0f - b}, {0.0f, b},
            };
            for (int i = 0; i < 8; i++) {
                const int next = (i + 1) % 8;
                const glm::vec3 b0(p[i].x, 0.0f, p[i].y), b1(p[next].x, 0.0f, p[next].y);
                const glm::vec3 t0(p[i].x, 1.0f, p[i].y), t1(p[next].x, 1.0f, p[next].y);
                drawList->AddLine(ProjectIcon(b0, center, scale), ProjectIcon(b1, center, scale), color, 1.5f);
                drawList->AddLine(ProjectIcon(t0, center, scale), ProjectIcon(t1, center, scale), color, 1.5f);
                drawList->AddLine(ProjectIcon(b0, center, scale), ProjectIcon(t0, center, scale), color, 1.5f);
            }
            return;
        }

        drawEdges(cubeVerts, cubeEdges, 12);
    }
}

BuildTool::BuildTool()
    : m_WireCube(MakeWireCubeLocal()),
      m_WireWedge(MakeWireWedgeLocal()),
      m_WireBeveledCube(MakeWireBeveledCubeLocal())
{
    m_WireCube.Upload();
    m_WireWedge.Upload();
    m_WireBeveledCube.Upload();
}

void BuildTool::Update(const Window& window, const Camera& camera, Chunk& chunk, Mesh& mesh) {
    m_LookHit = chunk.Raycast(camera.GetPosition(), camera.GetFront(), reach);
    m_CanPlace = false;

    if (m_LookHit.hit) {
        m_PlaceAt = m_LookHit.voxel + m_LookHit.normal;
        m_CanPlace =
            m_PlaceAt.x >= 0 && m_PlaceAt.x < chunk.sizeX &&
            m_PlaceAt.y >= 0 && m_PlaceAt.y < chunk.sizeY &&
            m_PlaceAt.z >= 0 && m_PlaceAt.z < chunk.sizeZ;
    }

    const auto w = window.GetNativeHandle();

    // Shape hotbar - instant-select, no edge detection needed since
    // re-selecting the same shape every frame it's held is harmless.
    if (glfwGetKey(w, GLFW_KEY_1) == GLFW_PRESS) m_SelectedShape = ShapeType::Cube;
    if (glfwGetKey(w, GLFW_KEY_2) == GLFW_PRESS) m_SelectedShape = ShapeType::Wedge;
    if (glfwGetKey(w, GLFW_KEY_3) == GLFW_PRESS) m_SelectedShape = ShapeType::BeveledCube;
    // 4 is intentionally unbound - only 3 shapes exist so far.

    // Rotation - edge-detected so it steps once per press instead of
    // spinning continuously while held.
    const bool qDown = glfwGetKey(w, GLFW_KEY_Q) == GLFW_PRESS;
    const bool eDown = glfwGetKey(w, GLFW_KEY_E) == GLFW_PRESS;
    if (qDown && !m_QWasDown) m_SelectedRotation = (m_SelectedRotation + 3) % 4;
    if (eDown && !m_EWasDown) m_SelectedRotation = (m_SelectedRotation + 1) % 4;
    m_QWasDown = qDown;
    m_EWasDown = eDown;

    const bool leftDown = glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    const bool rightDown = glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    const bool breakBlock = leftDown && !m_LeftWasDown;
    const bool placeBlock = rightDown && !m_RightWasDown;

    m_LeftWasDown = leftDown;
    m_RightWasDown = rightDown;

    if (!m_LookHit.hit || !(breakBlock || placeBlock)) {
        return;
    }

    bool edited = false;

    if (breakBlock) {
        chunk.voxels[chunk.Index(m_LookHit.voxel.x, m_LookHit.voxel.y, m_LookHit.voxel.z)] = 0;
        // Reset shape/rotation too - voxels[]/shapes[]/rotations[] are
        // separate arrays, so leaving these set would silently resurrect
        // the same piece the next time something gets placed in this cell.
        chunk.SetShape(m_LookHit.voxel.x, m_LookHit.voxel.y, m_LookHit.voxel.z, ShapeType::Cube);
        chunk.SetRotation(m_LookHit.voxel.x, m_LookHit.voxel.y, m_LookHit.voxel.z, 0);
        edited = true;
    } else if (m_CanPlace) {
        chunk.voxels[chunk.Index(m_PlaceAt.x, m_PlaceAt.y, m_PlaceAt.z)] = 1;
        chunk.SetShape(m_PlaceAt.x, m_PlaceAt.y, m_PlaceAt.z, m_SelectedShape);
        chunk.SetRotation(m_PlaceAt.x, m_PlaceAt.y, m_PlaceAt.z, m_SelectedRotation);
        edited = true;
    }

    if (edited) {
        auto [newVertices, newIndices] = ChunkMesher::BuildMesh(chunk);
        mesh.UpdateData(newVertices, newIndices);
    }
}

void BuildTool::DrawPreview(Shader& highlightShader, const glm::mat4& viewProjection) {
    if (!m_CanPlace) {
        return;
    }

    // Rotate around the cell's own vertical center, then move into place -
    // matches ChunkMesher::EmitWedge's RotateLocal usage exactly, so the
    // preview always lines up with what will actually get placed.
    const glm::mat4 transform =
        glm::translate(glm::mat4(1.0f), glm::vec3(m_PlaceAt)) *
        glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.5f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_SelectedRotation * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, -0.5f));

    Mesh* wireMesh = &m_WireCube;
    if (m_SelectedShape == ShapeType::Wedge) wireMesh = &m_WireWedge;
    else if (m_SelectedShape == ShapeType::BeveledCube) wireMesh = &m_WireBeveledCube;

    highlightShader.Bind();
    highlightShader.SetMat4("uTransform", viewProjection * transform);
    wireMesh->Draw(GL_LINES);
}

void BuildTool::DrawPalettePanel() {
    ImGui::SetNextWindowPos(ImVec2(16, 140), ImGuiCond_FirstUseEver);
    ImGui::Begin("Pieces", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

    constexpr ShapeType shapes[] = { ShapeType::Cube, ShapeType::Wedge, ShapeType::BeveledCube };
    constexpr float slotSize = 56.0f;
    constexpr ImU32 iconColor = IM_COL32(0, 255, 150, 255);

    for (int i = 0; i < 3; i++) {
        const ShapeType shape = shapes[i];
        const bool selected = (m_SelectedShape == shape);

        ImGui::PushID(i);
        ImGui::InvisibleButton("slot", ImVec2(slotSize, slotSize));
        if (ImGui::IsItemClicked()) {
            m_SelectedShape = shape;
        }

        const ImVec2 slotMin = ImGui::GetItemRectMin();
        const ImVec2 slotMax = ImGui::GetItemRectMax();
        const ImVec2 center((slotMin.x + slotMax.x) * 0.5f, (slotMin.y + slotMax.y) * 0.5f);

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(slotMin, slotMax, IM_COL32(20, 22, 26, 255), 4.0f);
        drawList->AddRect(
            slotMin, slotMax,
            selected ? iconColor : IM_COL32(70, 72, 80, 255),
            4.0f, 0, selected ? 2.5f : 1.0f);

        DrawShapeIcon(drawList, shape, center, slotSize * 0.3f, iconColor);

        ImGui::SameLine();
        ImGui::Text("%d: %s", i + 1, ShapeName(shape));
        ImGui::PopID();
    }

    ImGui::Separator();
    ImGui::Text("Rotation: %d deg (Q/E)", m_SelectedRotation * 90);

    ImGui::End();
}

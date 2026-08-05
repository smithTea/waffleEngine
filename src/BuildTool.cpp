//
// Created by smith on 06.08.26.
//

#include "headers/BuildTool.h"

#include "headers/ChunkMesher.h"

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

    const bool leftDown = glfwGetMouseButton(window.GetNativeHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    const bool rightDown = glfwGetMouseButton(window.GetNativeHandle(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

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
        edited = true;
    } else if (m_CanPlace) {
        chunk.voxels[chunk.Index(m_PlaceAt.x, m_PlaceAt.y, m_PlaceAt.z)] = 1;
        edited = true;
    }

    if (edited) {
        auto [newVertices, newIndices] = ChunkMesher::BuildMesh(chunk);
        mesh.UpdateData(newVertices, newIndices);
    }
}

void BuildTool::DrawPreview(Shader& highlightShader, Mesh& highlightMesh, const glm::mat4& viewProjection) const {
    if (!m_CanPlace) {
        return;
    }

    const glm::mat4 highlightTransform = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(m_PlaceAt) + glm::vec3(0.5f));

    highlightShader.Bind();
    highlightShader.SetMat4("uTransform", viewProjection * highlightTransform);
    highlightMesh.Draw(GL_LINES);
}

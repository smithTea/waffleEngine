//
// Created by smith on 06.08.26.
//

#ifndef WAFFLE_BUILDTOOL_H
#define WAFFLE_BUILDTOOL_H

#include <glm/glm.hpp>

#include "Camera.h"
#include "Chunk.h"
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

// Minecraft-style block break/place. Raycasts from the camera every frame
// (not just on click) so DrawPreview can track the crosshair continuously,
// then edits + remeshes the chunk when a mouse button is pressed.
class BuildTool {
public:
    void Update(const Window& window, const Camera& camera, Chunk& chunk, Mesh& mesh);
    void DrawPreview(Shader& highlightShader, Mesh& highlightMesh, const glm::mat4& viewProjection) const;

    float reach = 8.0f;

private:
    RaycastHit m_LookHit;
    glm::ivec3 m_PlaceAt{0};
    bool m_CanPlace = false;

    bool m_LeftWasDown = false;
    bool m_RightWasDown = false;
};

#endif //WAFFLE_BUILDTOOL_H

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
#include "VoxelShape.h"
#include "Window.h"

// Minecraft-style block break/place, with a small catalog of non-Cube
// shapes. Raycasts from the camera every frame (not just on click) so
// DrawPreview can track the crosshair continuously, then edits + remeshes
// the chunk when a mouse button is pressed.
class BuildTool {
public:
    BuildTool();

    void Update(const Window& window, const Camera& camera, Chunk& chunk, Mesh& mesh);
    void DrawPreview(Shader& highlightShader, const glm::mat4& viewProjection);

    // Left-side palette panel: one square per catalog shape with a small
    // wireframe icon, highlighting whichever is currently selected.
    void DrawPalettePanel();

    float reach = 8.0f;

private:
    RaycastHit m_LookHit;
    glm::ivec3 m_PlaceAt{0};
    bool m_CanPlace = false;

    bool m_LeftWasDown = false;
    bool m_RightWasDown = false;
    bool m_QWasDown = false;
    bool m_EWasDown = false;

    ShapeType m_SelectedShape = ShapeType::Cube;
    int m_SelectedRotation = 0;

    // Preview-only wireframes, one per catalog shape, in the same [0,1]^3
    // local space EmitWedge/EmitBeveledCube use - lets DrawPreview apply
    // one rotation transform formula uniformly across all of them.
    Mesh m_WireCube;
    Mesh m_WireWedge;
    Mesh m_WireBeveledCube;
};

#endif //WAFFLE_BUILDTOOL_H

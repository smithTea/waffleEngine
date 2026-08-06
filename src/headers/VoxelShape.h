//
// Created by smith on 06.08.26.
//

#ifndef WAFFLE_VOXELSHAPE_H
#define WAFFLE_VOXELSHAPE_H

#include <cstdint>
#include <glm/vec3.hpp>

// Connector contract: what a shape presents on each of its 6 axis-aligned
// cell faces (index order matches ChunkMesher::FACE_DIRS: 0=+X, 1=-X,
// 2=+Y, 3=-Y, 4=+Z, 5=-Z). Two adjacent solid cells only get their shared
// face culled when BOTH sides present a Full connector toward each other -
// anything else (a sloped ramp face, an open wedge front, a bevel corner)
// always renders, so gaps and chamfers stay visible instead of leaving a
// hole in the mesh.
enum class Connector : uint8_t {
    None = 0,
    Full = 1,
};

// Voxel value stored per-cell alongside the material ID (see Chunk::shapes).
// Cube is the default and is handled entirely by the existing greedy
// mesher; anything else is emitted as its own individual geometry by
// ChunkMesher::EmitCustomShapes.
enum class ShapeType : uint8_t {
    Cube = 0,
    Wedge = 1,        // ramp: full back/bottom/sides, open sloped top and low front
    BeveledCube = 2,  // cube with its 4 vertical edges chamfered
};

// Bevel amount shared between the solid mesh (ChunkMesher::EmitBeveledCube)
// and its wireframe preview icon (BuildTool), so they never drift apart.
constexpr float BEVELED_CUBE_BEVEL = 0.2f;

// rotation is 0-3, each step a 90-degree turn around +Y - the only axis
// that makes sense for pieces placed on a horizontal grid, since Y itself
// (up/down) never changes under it.

// Which world direction (see the index order above) a shape's LOCAL face
// ends up facing after `rotation` steps.
int RotatedFaceIndex(int localFaceIndex, int rotation);

// What connector a shape presents toward world direction worldFaceDirIndex
// once placed with the given rotation (internally un-rotates the query
// back to the shape's local space and looks that up instead).
Connector GetConnector(ShapeType shape, int worldFaceDirIndex, int rotation);

// Rotates a shape-local direction or (already center-relative) position
// offset by `rotation` steps. Positions defined in [0,1]^3 cell space need
// to be re-centered around (0.5, y, 0.5) before/after calling this - see
// ChunkMesher::EmitWedge for the pattern.
glm::vec3 RotateLocal(const glm::vec3& v, int rotation);

#endif //WAFFLE_VOXELSHAPE_H

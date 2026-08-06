//
// Created by smith on 06.08.26.
//

#include "headers/VoxelShape.h"

namespace {
    // Indexed like ChunkMesher::FACE_DIRS: 0=+X, 1=-X, 2=+Y, 3=-Y, 4=+Z, 5=-Z
    constexpr Connector CubeConnectors[6] = {
        Connector::Full, Connector::Full, Connector::Full,
        Connector::Full, Connector::Full, Connector::Full,
    };

    // +X/-X here are the wedge's triangular side caps - only half the
    // cell face, not a full seal. Marking those Full would cull a
    // neighbor's flat quad face and leave a real hole where the wedge
    // doesn't reach (the neighbor face doesn't get culled - see
    // ChunkMesher::EmitCustomShapes; but if it did, the gap left by the
    // wedge's missing corner would show through). Only the flat, fully
    // rectangular bottom and back wall are safe to mark Full.
    constexpr Connector WedgeConnectors[6] = {
        Connector::None,  // +X (triangular side cap, partial coverage)
        Connector::None,  // -X (triangular side cap, partial coverage)
        Connector::None,  // +Y - the sloped top, not a flat cap
        Connector::Full,  // -Y - full bottom square
        Connector::Full,  // +Z - full back wall (the tall end of the ramp)
        Connector::None,  // -Z - the low, open front of the ramp
    };

    // The bevel only chamfers the 4 *vertical* edges (see EmitBeveledCube),
    // which means the horizontal cross-section is the same inset octagon
    // at every height - including the top and bottom caps. None of the 6
    // faces are actually full squares, so none of them are safe to mark
    // Full; every direction stays None, same reasoning as the wedge.
    constexpr Connector BeveledCubeConnectors[6] = {
        Connector::None, Connector::None, Connector::None,
        Connector::None, Connector::None, Connector::None,
    };

    const Connector* ConnectorsFor(const ShapeType shape) {
        switch (shape) {
            case ShapeType::Wedge: return WedgeConnectors;
            case ShapeType::BeveledCube: return BeveledCubeConnectors;
            case ShapeType::Cube:
            default: return CubeConnectors;
        }
    }

    // Local->world face index after `rotation` 90-degree steps around +Y,
    // derived from the standard rotation-matrix formula x'=x*cos+z*sin,
    // z'=-x*sin+z*cos. Vertical faces (index 2,3 = +Y,-Y) never move;
    // horizontal faces cycle +X -> -Z -> -X -> +Z -> +X.
    constexpr int kRotationPermutation[4][6] = {
        {0, 1, 2, 3, 4, 5},
        {5, 4, 2, 3, 0, 1},
        {1, 0, 2, 3, 5, 4},
        {4, 5, 2, 3, 1, 0},
    };
}

int RotatedFaceIndex(const int localFaceIndex, const int rotation) {
    const int r = ((rotation % 4) + 4) % 4;
    return kRotationPermutation[r][localFaceIndex];
}

Connector GetConnector(const ShapeType shape, const int worldFaceDirIndex, const int rotation) {
    const int r = ((rotation % 4) + 4) % 4;
    const int inverse = (4 - r) % 4; // un-rotate the query back to local space
    const int localIndex = kRotationPermutation[inverse][worldFaceDirIndex];
    return ConnectorsFor(shape)[localIndex];
}

glm::vec3 RotateLocal(const glm::vec3& v, const int rotation) {
    const int r = ((rotation % 4) + 4) % 4;
    glm::vec3 result = v;
    for (int i = 0; i < r; i++) {
        result = glm::vec3(result.z, result.y, -result.x);
    }
    return result;
}

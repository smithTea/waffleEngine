//
// Created by smith on 20.07.26.
//

#include "../headers/ChunkMesher.h"

static const int dx[6] = { 1, -1, 0, 0, 0, 0 };
static const int dy[6] = { 0, 0, 1, -1, 0, 0 };
static const int dz[6] = { 0, 0, 0, 0, 1, -1 };

static void GetLayerDims(const Chunk& chunk,const int axis, int& layerCount, int& uMax, int& vMax) {
    switch (axis) {
        case 0: layerCount = chunk.sizeX; uMax = chunk.sizeY; vMax = chunk.sizeZ; break;
        case 1: layerCount = chunk.sizeY; uMax = chunk.sizeZ; vMax = chunk.sizeX; break;
        case 2: layerCount = chunk.sizeZ; uMax = chunk.sizeX; vMax = chunk.sizeY; break;
    }
}

static glm::ivec3 MakeCoord(const int axis,const int layer,const int u,const int v) {
    switch (axis) {
        case 0: return glm::ivec3(layer, u, v);
        case 1: return glm::ivec3(v, layer, u);
        case 2: return glm::ivec3(u, v, layer);
    }
    return glm::ivec3(0);
}

static glm::vec3 MakeCoordF(const int axis, const float layer,const float u, const float v) {
    switch (axis) {
        case 0: return glm::vec3(layer, u, v);
        case 1: return glm::vec3(v, layer, u);
        case 2: return glm::vec3(u, v, layer);
    }
    return glm::vec3(0);
}

void ChunkMesher::BuildMaskForLayer(const Chunk& chunk, const FaceDir& face, const int layer,
                                      int mask[MAX_CHUNK_DIM][MAX_CHUNK_DIM], const int uMax, const int vMax) {
    // Index into FACE_DIRS matching this face (0=+X,1=-X,2=+Y,3=-Y,4=+Z,5=-Z
    // - see the array below); needed to look up what the neighbor presents
    // back toward us, not just whether it's solid.
    const int faceDirIndex = face.axis * 2 + (face.dir > 0 ? 0 : 1);
    const int oppositeIndex = faceDirIndex ^ 1;

    for (int u = 0; u < uMax; u++) {
        for (int v = 0; v < vMax; v++) {
            glm::ivec3 pos = MakeCoord(face.axis, layer, u, v);
            const glm::ivec3 neighbor = pos + face.normal;

            const bool solidHere = chunk.isSolid(pos.x, pos.y, pos.z);
            const bool solidNeighbor = chunk.isSolid(neighbor.x, neighbor.y, neighbor.z);
            // Non-Cube shapes aren't uniform rectangles and can't go
            // through greedy merging - they're skipped here and picked
            // up individually by EmitCustomShapes instead.
            const bool isCube = chunk.GetShape(pos.x, pos.y, pos.z) == ShapeType::Cube;

            // A cube face is only sealed (and can be culled) if the
            // neighbor actually presents a Full connector back toward it -
            // being solid isn't enough, since a wedge's sloped side or a
            // beveled cube's chamfered face only cover part of the cell
            // and would otherwise wrongly hide this cube's face.
            bool sealedByNeighbor = false;
            if (solidNeighbor) {
                const ShapeType neighborShape = chunk.GetShape(neighbor.x, neighbor.y, neighbor.z);
                const int neighborRotation = chunk.GetRotation(neighbor.x, neighbor.y, neighbor.z);
                sealedByNeighbor = GetConnector(neighborShape, oppositeIndex, neighborRotation) == Connector::Full;
            }

            mask[u][v] = (solidHere && isCube && !sealedByNeighbor) ? chunk.voxels[chunk.Index(pos.x, pos.y, pos.z)] : 0;
        }
    }
}

void ChunkMesher::GreedyMergeLayer(int mask[MAX_CHUNK_DIM][MAX_CHUNK_DIM], const FaceDir& face, const int layer,
                                     MeshData& mesh, const int uMax, const int vMax, const int chunkHeight) {
    for (int u = 0; u < uMax; u++) {
        for (int v = 0; v < vMax; v++) {
            const int material = mask[u][v];
            if (material == 0) continue;

            int width = 1;
            while (v + width < vMax && mask[u][v + width] == material) width++;

            int height = 1;
            bool canExpand = true;
            while (u + height < uMax && canExpand) {
                for (int k = 0; k < width; k++)
                    if (mask[u + height][v + k] != material) { canExpand = false; break; }
                if (canExpand) height++;
            }

            EmitQuad(mesh, face, layer, u, v, width, height, material, chunkHeight);

            for (int du = 0; du < height; du++)
                for (int dv = 0; dv < width; dv++)
                    mask[u + du][v + dv] = 0;
        }
    }
}

void ChunkMesher::EmitQuad(MeshData& mesh, const FaceDir& face, const int layer,
                            const int u, const int v, const int width, const int height, int material, const int chunkHeight) {
    const float plane = (face.dir > 0) ? layer + 1.0f : static_cast<float>(layer);

    const glm::vec3 p0 = MakeCoordF(face.axis, plane, u, v);
    const glm::vec3 p1 = MakeCoordF(face.axis, plane, u, v + width);
    const glm::vec3 p2 = MakeCoordF(face.axis, plane, u + height, v + width);
    const glm::vec3 p3 = MakeCoordF(face.axis, plane, u + height, v);

    auto colorFor = [chunkHeight](const float worldY) {
        const float t = glm::clamp(worldY / static_cast<float>(chunkHeight), 0.0f, 1.0f);
        return glm::mix(glm::vec3(0.05f, 0.05f, 0.2f), glm::vec3(0.9f, 0.1f, 0.6f), t);
    };

    const glm::vec3 normal(face.normal);
    const auto materialID = static_cast<float>(material);
    const GLuint start = mesh.vertices.size();

    // Vertex order must trace CCW as seen from the direction the face normal
    // points, or GL_CULL_FACE/GL_BACK culls the face from its visible side
    // and only shows it from behind.
    if (face.dir > 0) {
        mesh.vertices.push_back({ p0, colorFor(p0.y), normal, materialID });
        mesh.vertices.push_back({ p3, colorFor(p3.y), normal, materialID });
        mesh.vertices.push_back({ p2, colorFor(p2.y), normal, materialID });
        mesh.vertices.push_back({ p1, colorFor(p1.y), normal, materialID });
    } else {
        mesh.vertices.push_back({ p0, colorFor(p0.y), normal, materialID });
        mesh.vertices.push_back({ p1, colorFor(p1.y), normal, materialID });
        mesh.vertices.push_back({ p2, colorFor(p2.y), normal, materialID });
        mesh.vertices.push_back({ p3, colorFor(p3.y), normal, materialID });
    }

    mesh.indices.push_back(start + 0);
    mesh.indices.push_back(start + 1);
    mesh.indices.push_back(start + 2);
    mesh.indices.push_back(start + 0);
    mesh.indices.push_back(start + 2);
    mesh.indices.push_back(start + 3);
}

void ChunkMesher::EmitFace(MeshData& mesh, const int x, const int y, const int z, const int face) {
    const glm::vec3 pos(x, y, z);

    const float t = pos.y / static_cast<float>(MAX_CHUNK_DIM);
    constexpr glm::vec3 colorBottom(0.05f, 0.05f, 0.2f);
    constexpr glm::vec3 colorTop(0.9f, 0.1f, 0.6f);
    const glm::vec3 color = glm::mix(colorBottom, colorTop, t);

    const glm::vec3 corners[6][4] = {
        // +X
        { {1,0,0}, {1,1,0}, {1,1,1}, {1,0,1} },
        // -X
        { {0,0,1}, {0,1,1}, {0,1,0}, {0,0,0} },
        // +Y
        { {0,1,0}, {0,1,1}, {1,1,1}, {1,1,0} },
        // -Y
        { {0,0,1}, {0,0,0}, {1,0,0}, {1,0,1} },
        // +Z
        { {1,0,1}, {1,1,1}, {0,1,1}, {0,0,1} },
        // -Z
        { {0,0,0}, {0,1,0}, {1,1,0}, {1,0,0} },
    };

    const GLuint startIndex = mesh.vertices.size();

    for (int i = 0; i < 4; i++) {
        mesh.vertices.push_back({ pos + corners[face][i], color });
       // mesh.indices.push_back(startIndex + i);
    }

    mesh.indices.push_back(startIndex + 0);
    mesh.indices.push_back(startIndex + 1);
    mesh.indices.push_back(startIndex + 2);

    mesh.indices.push_back(startIndex + 0);
    mesh.indices.push_back(startIndex + 2);
    mesh.indices.push_back(startIndex + 3);
}

void ChunkMesher::BuildDirection(const Chunk& chunk, const FaceDir& face, MeshData& mesh) {
    int layerCount, uMax, vMax;
    GetLayerDims(chunk, face.axis, layerCount, uMax, vMax);

    for (int layer = 0; layer < layerCount; layer++) {
        int mask[MAX_CHUNK_DIM][MAX_CHUNK_DIM];
        BuildMaskForLayer(chunk, face, layer, mask, uMax, vMax);
        GreedyMergeLayer(mask, face, layer, mesh, uMax, vMax, chunk.sizeY);
    }
}

const ChunkMesher::FaceDir ChunkMesher::FACE_DIRS[6] = {
    { 0,  1, {1,0,0} },   // +X
    { 0, -1, {-1,0,0} },  // -X
    { 1,  1, {0,1,0} },   // +Y
    { 1, -1, {0,-1,0} },  // -Y
    { 2,  1, {0,0,1} },   // +Z
    { 2, -1, {0,0,-1} },  // -Z
};

MeshData ChunkMesher::BuildMesh(const Chunk& chunk) {
    MeshData mesh;
    for (const auto& face : FACE_DIRS) {
        BuildDirection(chunk, face, mesh);
    }
    EmitCustomShapes(chunk, mesh);
    return mesh;
}

namespace {
    // vColor is currently unused by the chunk fragment shader (materials
    // fully replaced it as the color source - see triangle.frag), so
    // custom shapes don't bother computing a height tint for it the way
    // EmitQuad does; it's just a placeholder.
    void AddTriangle(MeshData& mesh, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
                      const glm::vec3& normal, const float materialID) {
        const GLuint start = mesh.vertices.size();
        mesh.vertices.push_back({ a, glm::vec3(1.0f), normal, materialID });
        mesh.vertices.push_back({ b, glm::vec3(1.0f), normal, materialID });
        mesh.vertices.push_back({ c, glm::vec3(1.0f), normal, materialID });
        mesh.indices.push_back(start + 0);
        mesh.indices.push_back(start + 1);
        mesh.indices.push_back(start + 2);
    }

    void AddQuad(MeshData& mesh, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d,
                 const glm::vec3& normal, const float materialID) {
        AddTriangle(mesh, a, b, c, normal, materialID);
        AddTriangle(mesh, a, c, d, normal, materialID);
    }
}

// Ramp: rises from zero height at -Z (the open, low front) to full height
// at +Z (the back wall) in its unrotated (rotation=0) orientation. Only
// the bottom and back wall are full flat squares, so only those two
// respect cullFace - see VoxelShape.cpp for why.
void ChunkMesher::EmitWedge(MeshData& mesh, const glm::ivec3& cellPos, const int rotation, const float materialID, const bool cullFace[6]) {
    const glm::vec3 origin(cellPos);
    constexpr glm::vec3 center(0.5f, 0.0f, 0.5f);

    // Positions are defined at rotation 0 and rotated around the cell's
    // own vertical center axis; normals just rotate in place (no center
    // offset - they're directions, not positions).
    auto RP = [&](const glm::vec3& local) { return origin + RotateLocal(local - center, rotation) + center; };
    auto RN = [&](const glm::vec3& dir) { return RotateLocal(dir, rotation); };

    const glm::vec3 A = RP(glm::vec3(0, 0, 0));
    const glm::vec3 B = RP(glm::vec3(1, 0, 0));
    const glm::vec3 C = RP(glm::vec3(1, 0, 1));
    const glm::vec3 D = RP(glm::vec3(0, 0, 1));
    const glm::vec3 E = RP(glm::vec3(0, 1, 1));
    const glm::vec3 F = RP(glm::vec3(1, 1, 1));

    if (!cullFace[3]) // local -Y bottom - vertical faces never move under rotation
        AddQuad(mesh, A, B, C, D, RN(glm::vec3(0, -1, 0)), materialID);

    if (!cullFace[RotatedFaceIndex(4, rotation)]) // local +Z back wall
        AddQuad(mesh, D, C, F, E, RN(glm::vec3(0, 0, 1)), materialID);

    // Triangular side caps and the ramp surface itself never seal against
    // a neighbor (see WedgeConnectors), so they always render.
    AddTriangle(mesh, A, D, E, RN(glm::vec3(-1, 0, 0)), materialID);
    AddTriangle(mesh, B, F, C, RN(glm::vec3(1, 0, 0)), materialID);
    AddQuad(mesh, A, E, F, B, RN(glm::normalize(glm::vec3(0, 1, -1))), materialID);
}

// Cube with its 4 vertical edges chamfered - flat top/bottom octagon caps
// connected by 8 side strips (4 narrowed cardinal faces, 4 diagonal
// corner faces). None of its faces are ever culled (see VoxelShape.cpp),
// and the octagon is 4-fold symmetric under 90-degree turns, so rotation
// would be a visual no-op - not applied here.
void ChunkMesher::EmitBeveledCube(MeshData& mesh, const glm::ivec3& cellPos, const float materialID, const bool cullFace[6]) {
    const glm::vec3 origin(cellPos);
    constexpr float bevel = BEVELED_CUBE_BEVEL;

    const glm::vec2 p[8] = {
        {bevel, 0.0f}, {1.0f - bevel, 0.0f}, {1.0f, bevel}, {1.0f, 1.0f - bevel},
        {1.0f - bevel, 1.0f}, {bevel, 1.0f}, {0.0f, 1.0f - bevel}, {0.0f, bevel},
    };

    auto at = [&](const int i, const float y) {
        return origin + glm::vec3(p[i].x, y, p[i].y);
    };

    if (!cullFace[3]) { // -Y bottom cap, fan from p0
        for (int i = 1; i < 7; i++)
            AddTriangle(mesh, at(0, 0.0f), at(i, 0.0f), at(i + 1, 0.0f), glm::vec3(0, -1, 0), materialID);
    }

    if (!cullFace[2]) { // +Y top cap, fan from p7 (reversed winding vs. the bottom)
        for (int i = 6; i > 0; i--)
            AddTriangle(mesh, at(7, 1.0f), at(i, 1.0f), at(i - 1, 1.0f), glm::vec3(0, 1, 0), materialID);
    }

    // 8 side strips around the octagon; always drawn, never culled.
    for (int i = 0; i < 8; i++) {
        const int next = (i + 1) % 8;
        const glm::vec3 b0 = at(i, 0.0f), b1 = at(next, 0.0f), t0 = at(i, 1.0f), t1 = at(next, 1.0f);
        const glm::vec3 normal = glm::normalize(glm::cross(b0 - b1, t0 - b1));
        AddQuad(mesh, b1, b0, t0, t1, normal, materialID);
    }
}

void ChunkMesher::EmitVoxelShape(MeshData& mesh, const ShapeType shape, const glm::ivec3& cellPos, const int rotation, const float materialID, const bool cullFace[6]) {
    switch (shape) {
        case ShapeType::Wedge:
            EmitWedge(mesh, cellPos, rotation, materialID, cullFace);
            break;
        case ShapeType::BeveledCube:
            EmitBeveledCube(mesh, cellPos, materialID, cullFace);
            break;
        case ShapeType::Cube:
        default:
            break; // handled entirely by the greedy mesher above
    }
}

void ChunkMesher::EmitCustomShapes(const Chunk& chunk, MeshData& mesh) {
    for (int x = 0; x < chunk.sizeX; x++) {
        for (int y = 0; y < chunk.sizeY; y++) {
            for (int z = 0; z < chunk.sizeZ; z++) {
                if (!chunk.isSolid(x, y, z)) continue;

                const ShapeType shape = chunk.GetShape(x, y, z);
                if (shape == ShapeType::Cube) continue; // already meshed above

                const int rotation = chunk.GetRotation(x, y, z);

                bool cullFace[6];
                for (int i = 0; i < 6; i++) {
                    const FaceDir& dir = FACE_DIRS[i];
                    const glm::ivec3 neighborPos(x + dir.normal.x, y + dir.normal.y, z + dir.normal.z);
                    const bool neighborSolid = chunk.isSolid(neighborPos.x, neighborPos.y, neighborPos.z);

                    const Connector myConnector = GetConnector(shape, i, rotation);
                    Connector neighborConnector = Connector::None;
                    if (neighborSolid) {
                        const ShapeType neighborShape = chunk.GetShape(neighborPos.x, neighborPos.y, neighborPos.z);
                        const int neighborRotation = chunk.GetRotation(neighborPos.x, neighborPos.y, neighborPos.z);
                        neighborConnector = GetConnector(neighborShape, i ^ 1, neighborRotation); // i^1 flips within each +/- pair
                    }

                    cullFace[i] = neighborSolid && myConnector == Connector::Full && neighborConnector == Connector::Full;
                }

                const auto materialID = static_cast<float>(chunk.voxels[chunk.Index(x, y, z)]);
                EmitVoxelShape(mesh, shape, glm::ivec3(x, y, z), rotation, materialID, cullFace);
            }
        }
    }
}
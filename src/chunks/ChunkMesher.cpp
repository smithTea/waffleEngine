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
    for (int u = 0; u < uMax; u++) {
        for (int v = 0; v < vMax; v++) {
            glm::ivec3 pos = MakeCoord(face.axis, layer, u, v);
            const glm::ivec3 neighbor = pos + face.normal;

            const bool solidHere = chunk.isSolid(pos.x, pos.y, pos.z);
            const bool solidNeighbor = chunk.isSolid(neighbor.x, neighbor.y, neighbor.z);

            mask[u][v] = (solidHere && !solidNeighbor) ? chunk.voxels[chunk.Index(pos.x, pos.y, pos.z)] : 0;
        }
    }
}

void ChunkMesher::GreedyMergeLayer(int mask[MAX_CHUNK_DIM][MAX_CHUNK_DIM], const FaceDir& face, const int layer,
                                     MeshData& mesh, const int uMax, const int vMax) {
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

            EmitQuad(mesh, face, layer, u, v, width, height, material);

            for (int du = 0; du < height; du++)
                for (int dv = 0; dv < width; dv++)
                    mask[u + du][v + dv] = 0;
        }
    }
}

void ChunkMesher::EmitQuad(MeshData& mesh, const FaceDir& face, const int layer,
                            const int u, const int v, const int width, const int height, int material) {
    const float plane = (face.dir > 0) ? layer + 1.0f : static_cast<float>(layer);

    const glm::vec3 p0 = MakeCoordF(face.axis, plane, u, v);
    const glm::vec3 p1 = MakeCoordF(face.axis, plane, u, v + width);
    const glm::vec3 p2 = MakeCoordF(face.axis, plane, u + height, v + width);
    const glm::vec3 p3 = MakeCoordF(face.axis, plane, u + height, v);

    auto colorFor = [](const float worldY) {
        const float t = glm::clamp(worldY / static_cast<float>(MAX_CHUNK_DIM), 0.0f, 1.0f);
        return glm::mix(glm::vec3(0.05f, 0.05f, 0.2f), glm::vec3(0.9f, 0.1f, 0.6f), t);
    };

    const GLuint start = mesh.vertices.size();

    if (face.dir > 0) {
        mesh.vertices.push_back({ p0, colorFor(p0.y) });
        mesh.vertices.push_back({ p1, colorFor(p1.y) });
        mesh.vertices.push_back({ p2, colorFor(p2.y) });
        mesh.vertices.push_back({ p3, colorFor(p3.y) });
    } else {
        mesh.vertices.push_back({ p0, colorFor(p0.y) });
        mesh.vertices.push_back({ p3, colorFor(p3.y) });
        mesh.vertices.push_back({ p2, colorFor(p2.y) });
        mesh.vertices.push_back({ p1, colorFor(p1.y) });
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
        GreedyMergeLayer(mask, face, layer, mesh, uMax, vMax);
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
    return mesh;
}
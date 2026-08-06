//
// Created by smith on 20.07.26.
//

#ifndef WAFFLE_CHUNKMESHER_H
#define WAFFLE_CHUNKMESHER_H

#include "Chunk.h"
#include "Mesh.h"
#include "VoxelShape.h"

struct FaceDir {
    int axis;
    int dir;
    int normal[3];
};
static const FaceDir FACE_DIRS[6] = {
    { 0,  1, { 1, 0, 0} }, // +X
    { 0, -1, {-1, 0, 0} }, // -X
    { 1,  1, { 0, 1, 0} }, // +Y
    { 1, -1, { 0,-1, 0} }, // -Y
    { 2,  1, { 0, 0, 1} }, // +Z
    { 2, -1, { 0, 0,-1} }, // -Z
};


class ChunkMesher {
public:
    static MeshData BuildMesh(const Chunk& chunk);

private:
    static void EmitFace(MeshData& mesh, int x, int y, int z, int face);

    struct FaceDir {
        int axis;       // 0=X, 1=Y, 2=Z,
        int dir;        // +1 || -1
        glm::ivec3 normal;
    };
    static const FaceDir FACE_DIRS[6];

    // ChunkMesher.h
    static constexpr int MAX_CHUNK_DIM = 64;

    static void BuildDirection(const Chunk& chunk, const FaceDir& face, MeshData& mesh);
    static void BuildMaskForLayer(const Chunk& chunk, const FaceDir& face, int layer,
                                    int mask[MAX_CHUNK_DIM][MAX_CHUNK_DIM], int uMax, int vMax);
    static void GreedyMergeLayer(int mask[MAX_CHUNK_DIM][MAX_CHUNK_DIM], const FaceDir& face, int layer,
                                   MeshData& mesh, int uMax, int vMax, int chunkHeight);
    static void EmitQuad(MeshData& mesh, const FaceDir& face, int layer,
                          int u, int v, int width, int height, int material, int chunkHeight);

    // Non-Cube shapes aren't uniform rectangles, so they can't go through
    // the greedy mask/merge path above - each gets its own individual
    // geometry instead, emitted once per voxel after the greedy pass.
    static void EmitCustomShapes(const Chunk& chunk, MeshData& mesh);
    static void EmitVoxelShape(MeshData& mesh, ShapeType shape, const glm::ivec3& cellPos, int rotation, float materialID, const bool cullFace[6]);
    static void EmitWedge(MeshData& mesh, const glm::ivec3& cellPos, int rotation, float materialID, const bool cullFace[6]);
    static void EmitBeveledCube(MeshData& mesh, const glm::ivec3& cellPos, float materialID, const bool cullFace[6]);
};


#endif //WAFFLE_CHUNKMESHER_H
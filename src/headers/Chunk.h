//
// Created by smith on 20.07.26.
//

#ifndef WAFFLE_CHUNK_H
#define WAFFLE_CHUNK_H
#include <vector>
#include <glm/glm.hpp>

#include "VoxelShape.h"

struct RaycastHit {
    bool hit = false;
    glm::ivec3 voxel{0};  // the solid voxel that was hit
    glm::ivec3 normal{0}; // which face was hit, pointing away from the voxel
};

class Chunk {
public:
    static constexpr int MAX_SIZE = 32;
    int sizeX, sizeY, sizeZ;
    std::vector<uint8_t> voxels;    // flattened 3D array; material ID per voxel, 0 = air
    std::vector<uint8_t> shapes;    // ShapeType per voxel, defaults to Cube
    std::vector<uint8_t> rotations; // 0-3, defaults to 0 (see VoxelShape.h)

    Chunk(const int sx, const int sy, const int sz)
        : sizeX(sx), sizeY(sy), sizeZ(sz),
          voxels(sx * sy * sz, 0),
          shapes(sx * sy * sz, static_cast<uint8_t>(ShapeType::Cube)),
          rotations(sx * sy * sz, 0) {}

    inline int Index(const int x, const int y, const int z) const {
        return x + y * sizeX + z * sizeX * sizeY;
    }

    bool isSolid(int x, int y, int z) const;

    void Clear();

    void GenerateFlatTerrain(int h);
    void GenerateHollowRoom(int w);
    void GeneratePillarField(int floorHeight, int spacing, int pillarHeight);

    RaycastHit Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const;

    ShapeType GetShape(int x, int y, int z) const;
    void SetShape(int x, int y, int z, ShapeType shape);

    int GetRotation(int x, int y, int z) const;
    void SetRotation(int x, int y, int z, int rotation);
};


#endif //WAFFLE_CHUNK_H
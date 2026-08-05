//
// Created by smith on 20.07.26.
//

#ifndef WAFFLE_CHUNK_H
#define WAFFLE_CHUNK_H
#include <vector>
#include <glm/glm.hpp>

struct RaycastHit {
    bool hit = false;
    glm::ivec3 voxel{0};  // the solid voxel that was hit
    glm::ivec3 normal{0}; // which face was hit, pointing away from the voxel
};

class Chunk {
public:
    static constexpr int MAX_SIZE = 32;
    int sizeX, sizeY, sizeZ;
    std::vector<uint8_t> voxels; // flattened 3D array

    Chunk(const int sx, const int sy, const int sz)
        : sizeX(sx), sizeY(sy), sizeZ(sz), voxels(sx * sy * sz, 0) {}

    inline int Index(const int x, const int y, const int z) const {
        return x + y * sizeX + z * sizeX * sizeY;
    }

    bool isSolid(int x, int y, int z) const;

    void Clear();

    void GenerateFlatTerrain(int h);
    void GenerateHollowRoom(int w);
    void GeneratePillarField(int floorHeight, int spacing, int pillarHeight);

    RaycastHit Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const;
};


#endif //WAFFLE_CHUNK_H
//
// Created by smith on 20.07.26.
//

#include "../headers/Chunk.h"

#include <cmath>
#include <limits>

bool Chunk::isSolid(const int x, const int y, const int z) const {
    if (x < 0 || y < 0 || z < 0 || x >= sizeX || y >= sizeY || z >= sizeZ)
        return false;
    return voxels[Index(x, y, z)] != 0;
}


void Chunk::GenerateFlatTerrain(const int h) {
    for (int x = 0; x < MAX_SIZE; x++)
        for (int y = 0; y < MAX_SIZE; y++)
            for (int z = 0; z < MAX_SIZE; z++)
                voxels[Index(x,y,z)] = (y < h) ? 1 : 0;
}

void Chunk::GenerateHollowRoom(const int w) {
    for (int x = 0; x < sizeX; x++)
        for (int y = 0; y < sizeY; y++)
            for (int z = 0; z < sizeZ; z++) {
                bool isWall =
                    x < w || x >= sizeX - w ||
                    y < w || y >= sizeY - w ||
                    z < w || z >= sizeZ - w;

                 voxels[Index(x,y,z)] = isWall ? 1 : 0;
            }
}


// Amanatides-Woo grid traversal: steps voxel-by-voxel along the ray,
// checking each cell it enters until it hits a solid voxel or runs
// past maxDistance.
RaycastHit Chunk::Raycast(const glm::vec3& origin, const glm::vec3& direction, const float maxDistance) const {
    const glm::vec3 dir = glm::normalize(direction);
    glm::ivec3 voxel = glm::floor(origin);

    const glm::ivec3 step(
        dir.x > 0.0f ? 1 : -1,
        dir.y > 0.0f ? 1 : -1,
        dir.z > 0.0f ? 1 : -1);

    constexpr float infinity = std::numeric_limits<float>::max();
    const glm::vec3 tDelta(
        dir.x != 0.0f ? std::abs(1.0f / dir.x) : infinity,
        dir.y != 0.0f ? std::abs(1.0f / dir.y) : infinity,
        dir.z != 0.0f ? std::abs(1.0f / dir.z) : infinity);

    auto boundaryDist = [](const float originCoord, const int voxelCoord, const int stepDir) {
        return stepDir > 0 ? (static_cast<float>(voxelCoord) + 1.0f - originCoord) : (originCoord - static_cast<float>(voxelCoord));
    };

    glm::vec3 tMax(
        boundaryDist(origin.x, voxel.x, step.x) * tDelta.x,
        boundaryDist(origin.y, voxel.y, step.y) * tDelta.y,
        boundaryDist(origin.z, voxel.z, step.z) * tDelta.z);

    glm::ivec3 normal(0);

    if (isSolid(voxel.x, voxel.y, voxel.z)) {
        return { true, voxel, normal };
    }

    float traveled = 0.0f;
    while (traveled < maxDistance) {
        if (tMax.x < tMax.y && tMax.x < tMax.z) {
            voxel.x += step.x;
            traveled = tMax.x;
            tMax.x += tDelta.x;
            normal = glm::ivec3(-step.x, 0, 0);
        } else if (tMax.y < tMax.z) {
            voxel.y += step.y;
            traveled = tMax.y;
            tMax.y += tDelta.y;
            normal = glm::ivec3(0, -step.y, 0);
        } else {
            voxel.z += step.z;
            traveled = tMax.z;
            tMax.z += tDelta.z;
            normal = glm::ivec3(0, 0, -step.z);
        }

        if (isSolid(voxel.x, voxel.y, voxel.z)) {
            return { true, voxel, normal };
        }
    }

    return { false, glm::ivec3(0), glm::ivec3(0) };
}


void Chunk::GeneratePillarField(const int floorHeight, const int spacing, const int pillarHeight) {
    Clear();

    constexpr uint8_t floorMaterial = 1;
    constexpr uint8_t wallMaterial = 2;

    for (int x = 0; x < sizeX; x++)
        for (int z = 0; z < sizeZ; z++)
            for (int y = 0; y < floorHeight; y++)
                voxels[Index(x, y, z)] = floorMaterial;

    for (int x = spacing; x < sizeX - spacing; x += spacing)
        for (int z = spacing; z < sizeZ - spacing; z += spacing)
            for (int y = floorHeight; y < floorHeight + pillarHeight && y < sizeY; y++)
                voxels[Index(x, y, z)] = wallMaterial;
}


void Chunk::Clear() {
    std::fill(voxels.begin(), voxels.end(), 0);
    std::fill(shapes.begin(), shapes.end(), static_cast<uint8_t>(ShapeType::Cube));
    std::fill(rotations.begin(), rotations.end(), 0);
}

ShapeType Chunk::GetShape(const int x, const int y, const int z) const {
    if (x < 0 || y < 0 || z < 0 || x >= sizeX || y >= sizeY || z >= sizeZ)
        return ShapeType::Cube;
    return static_cast<ShapeType>(shapes[Index(x, y, z)]);
}

void Chunk::SetShape(const int x, const int y, const int z, const ShapeType shape) {
    shapes[Index(x, y, z)] = static_cast<uint8_t>(shape);
}

int Chunk::GetRotation(const int x, const int y, const int z) const {
    if (x < 0 || y < 0 || z < 0 || x >= sizeX || y >= sizeY || z >= sizeZ)
        return 0;
    return rotations[Index(x, y, z)];
}

void Chunk::SetRotation(const int x, const int y, const int z, const int rotation) {
    rotations[Index(x, y, z)] = static_cast<uint8_t>(((rotation % 4) + 4) % 4);
}
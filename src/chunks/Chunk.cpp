//
// Created by smith on 20.07.26.
//

#include "../headers/Chunk.h"

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


void Chunk::Clear() {
    std::fill(voxels.begin(), voxels.end(), 0);
}
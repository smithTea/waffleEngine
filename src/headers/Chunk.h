//
// Created by smith on 20.07.26.
//

#ifndef WAFFLE_CHUNK_H
#define WAFFLE_CHUNK_H
#include <vector>
#include <glm/fwd.hpp>

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
};


#endif //WAFFLE_CHUNK_H
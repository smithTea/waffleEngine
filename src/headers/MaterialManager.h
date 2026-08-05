//
// Created by smith on 05.08.26.
//

#ifndef WAFFLE_MATERIALMANAGER_H
#define WAFFLE_MATERIALMANAGER_H

#include <array>

#include "Material.h"
#include "Shader.h"

// Slot 0 is reserved (voxel value 0 means "air", never meshed), so
// materials are addressed by voxel value starting at 1.
class MaterialManager {
public:
    static constexpr int MAX_MATERIALS = 8;

    Material& Get(int id) { return m_Materials[id]; }

    void Update(float deltaTime);
    void Apply(Shader& shader) const;

private:
    std::array<Material, MAX_MATERIALS> m_Materials;
};

#endif //WAFFLE_MATERIALMANAGER_H

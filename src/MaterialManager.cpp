//
// Created by smith on 05.08.26.
//

#include "headers/MaterialManager.h"

#include <string>

void MaterialManager::Update(const float deltaTime) {
    for (auto& material : m_Materials) {
        material.chladni.Update(deltaTime);
    }
}

void MaterialManager::Apply(Shader& shader) const {
    for (size_t i = 0; i < m_Materials.size(); ++i) {
        const std::string prefix = "uMaterials[" + std::to_string(i) + "].";
        const Material& material = m_Materials[i];

        shader.SetVec3(prefix + "baseColor", material.baseColor);
        shader.SetVec3(prefix + "nodalColor", material.nodalColor);
        shader.SetFloat(prefix + "bumpStrength", material.bumpStrength);
        shader.SetFloat(prefix + "lineWidth", material.lineWidth);
        shader.SetFloat(prefix + "scale", material.scale);
        shader.SetFloat(prefix + "nA", material.chladni.GetFromN());
        shader.SetFloat(prefix + "mA", material.chladni.GetFromM());
        shader.SetFloat(prefix + "nB", material.chladni.GetToN());
        shader.SetFloat(prefix + "mB", material.chladni.GetToM());
        shader.SetFloat(prefix + "blend", material.chladni.GetBlend());
    }
}

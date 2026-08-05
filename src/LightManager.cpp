//
// Created by smith on 31.07.26.
//

#include "headers/LightManager.h"

#include <iostream>
#include <string>

void LightManager::AddDirectionalLight(
    const glm::vec3& direction,
    const glm::vec3& color,
    const float intensity)
{
    if (m_Lights.size() >= MAX_LIGHTS) {
        std::cout << "LightManager: MAX_LIGHTS reached, ignoring directional light\n";
        return;
    }

    Light light;
    light.type = LightType::Directional;
    light.direction = direction;
    light.color = color;
    light.intensity = intensity;
    m_Lights.push_back(light);
}

void LightManager::AddPointLight(
    const glm::vec3& position,
    const glm::vec3& color,
    const float intensity,
    const float radius)
{
    if (m_Lights.size() >= MAX_LIGHTS) {
        std::cout << "LightManager: MAX_LIGHTS reached, ignoring point light\n";
        return;
    }

    Light light;
    light.type = LightType::Point;
    light.position = position;
    light.color = color;
    light.intensity = intensity;
    light.radius = radius;
    m_Lights.push_back(light);
}

void LightManager::Apply(Shader& shader) const {
    shader.SetFloat("uAmbientStrength", ambientStrength);
    shader.SetInt("uLightCount", static_cast<int>(m_Lights.size()));

    for (size_t i = 0; i < m_Lights.size(); ++i) {
        const std::string prefix = "uLights[" + std::to_string(i) + "].";
        const Light& light = m_Lights[i];

        shader.SetInt(prefix + "type", static_cast<int>(light.type));
        shader.SetVec3(prefix + "direction", light.direction);
        shader.SetVec3(prefix + "position", light.position);
        shader.SetVec3(prefix + "color", light.color);
        shader.SetFloat(prefix + "intensity", light.intensity);
        shader.SetFloat(prefix + "radius", light.radius);
    }
}

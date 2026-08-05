//
// Created by smith on 31.07.26.
//

#ifndef WAFFLE_LIGHTMANAGER_H
#define WAFFLE_LIGHTMANAGER_H

#include <vector>
#include "Light.h"
#include "Shader.h"

class LightManager {
public:
    static constexpr int MAX_LIGHTS = 8;

    void AddDirectionalLight(
        const glm::vec3& direction,
        const glm::vec3& color = glm::vec3(1.0f),
        float intensity = 0.6f);

    void AddPointLight(
        const glm::vec3& position,
        const glm::vec3& color = glm::vec3(1.0f),
        float intensity = 1.0f,
        float radius = 10.0f);

    void Apply(Shader& shader) const;

    float ambientStrength = 0.4f;

private:
    std::vector<Light> m_Lights;
};

#endif //WAFFLE_LIGHTMANAGER_H

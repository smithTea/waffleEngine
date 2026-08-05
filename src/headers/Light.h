//
// Created by smith on 31.07.26.
//

#ifndef WAFFLE_LIGHT_H
#define WAFFLE_LIGHT_H

#include <glm/vec3.hpp>

enum class LightType {
    Directional,
    Point
};

struct Light {
    LightType type = LightType::Directional;
    glm::vec3 direction = glm::vec3(0.5f, 1.0f, 0.3f); // used when Directional
    glm::vec3 position  = glm::vec3(0.0f);              // used when Point
    glm::vec3 color     = glm::vec3(1.0f);
    float intensity = 0.6f;  // brightness of this light's lit faces
    float radius    = 10.0f; // point light falloff distance
};

#endif //WAFFLE_LIGHT_H

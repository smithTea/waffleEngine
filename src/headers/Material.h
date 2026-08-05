//
// Created by smith on 05.08.26.
//

#ifndef WAFFLE_MATERIAL_H
#define WAFFLE_MATERIAL_H

#include <glm/vec3.hpp>

#include "ChladniController.h"

// A surface look: colors plus its own Chladni pattern controller, so a
// wall material and a floor material can hold completely different
// figures and transition at completely different speeds.
struct Material {
    glm::vec3 baseColor{1.0f};
    glm::vec3 nodalColor{1.0f};
    float bumpStrength = 0.15f;
    float lineWidth = 0.05f;
    float scale = 1.0f;
    ChladniController chladni;
};

#endif //WAFFLE_MATERIAL_H

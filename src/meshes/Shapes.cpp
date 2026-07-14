//
// Created by smith on 14.07.26.
//

#include "../headers/Shapes.h"

Mesh MakePyramid()
{
    std::vector<Vertex> vertices = {
        { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f) },
        { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f) },
        { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f) },
        { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 1.0f, 0.0f) },
        { glm::vec3( 0.0f,  0.5f,  0.0f), glm::vec3(1.0f, 1.0f, 1.0f) },
    };
    std::vector<GLuint> indices = {
        0, 1, 2,  0, 2, 3,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4,
    };
    return Mesh(std::move(vertices), std::move(indices));
}
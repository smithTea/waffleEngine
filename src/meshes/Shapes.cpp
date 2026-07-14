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
Mesh MakeCube()
{
    std::vector<Vertex> vertices = {
        // Position                         // Color
        { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f) }, // 0
        { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f) }, // 1
        { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f) }, // 2
        { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 0.0f) }, // 3
        { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 1.0f) }, // 4
        { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 1.0f) }, // 5
        { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 1.0f, 1.0f) }, // 6
        { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.5f, 0.5f, 0.5f) }, // 7
    };

    std::vector<GLuint> indices = {
        // Front (+Z)
        4, 5, 6,
        4, 6, 7,

        // Back (-Z)
        0, 2, 1,
        0, 3, 2,

        // Left (-X)
        0, 4, 7,
        0, 7, 3,

        // Right (+X)
        1, 2, 6,
        1, 6, 5,

        // Top (+Y)
        3, 7, 6,
        3, 6, 2,

        // Bottom (-Y)
        0, 1, 5,
        0, 5, 4
    };

    return Mesh(std::move(vertices), std::move(indices));
}
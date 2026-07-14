//
// Created by smith on 13.07.26.
//

#include "headers/Mesh.h"

void CreateVertexArrays(GLuint m_VAO) {
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
}

void CreateBuffers(GLuint m_VBO, std::vector<Vertex> m_Vertices) {
    // Ask the GPU to create one vertex buffer
    glGenBuffers(1, &m_VBO);
    // Make this buffer the active vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    // Copy vertex data from CPU memory to GPU memory
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::Upload()
{
    CreateVertexArrays(m_VAO);
    CreateBuffers(m_VBO, m_Vertices);
    glEnableVertexAttribArray(0);
}

void Mesh::Draw()
{
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_Vertices.size());
}

Mesh::Mesh(std::vector<Vertex> vertices)
    : m_Vertices(std::move(vertices)) {

}
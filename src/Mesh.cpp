//
// Created by smith on 13.07.26.
//

#include "headers/Mesh.h"

#include <iostream>

void CreateVertexArrays(GLuint &m_VAO) {
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
}


void CreatePositionBuffer(GLuint &m_VBO, const std::vector<Vertex> &m_Vertices)
{
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, Position)));

    glEnableVertexAttribArray(0);
}

void CreateColorBuffer(GLuint &m_VBO, const std::vector<Vertex> &m_Vertices)
{
    glVertexAttribPointer(
    1,
    3,
    GL_FLOAT,
    GL_FALSE,
    sizeof(Vertex),
    reinterpret_cast<void*>(offsetof(Vertex, Color)));

    glEnableVertexAttribArray(1);
}


void CreateBuffers(GLuint &m_VBO, GLuint &m_EBO, const std::vector<Vertex> &m_Vertices, const std::vector<GLuint> &m_Indices)
{
    // Ask the GPU to create one vertex buffer
    glGenBuffers(1, &m_VBO);

    // Make this buffer the active vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // Copy vertex data from CPU memory to GPU memory
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_STATIC_DRAW);

    CreatePositionBuffer(m_VBO, m_Vertices);
    CreateColorBuffer(m_VBO, m_Vertices);

    if (!m_Indices.empty()) {
        glGenBuffers(1, &m_EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(GLuint), m_Indices.data(), GL_STATIC_DRAW);
    }


    // Clear
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Mesh::Upload()
{
    CreateVertexArrays(m_VAO);
    CreateBuffers(m_VBO, m_EBO, m_Vertices, m_Indices);
    glEnableVertexAttribArray(0);
}

void Mesh::Draw()
{
    glBindVertexArray(m_VAO);

    if (!m_Indices.empty()) glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, nullptr);
    else glDrawArrays(GL_TRIANGLES, 0, m_Vertices.size());
}

void Mesh::AttachInstanceBuffer(const InstanceBuffer &) {
    glBindVertexArray(m_VAO);
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_Vertices(std::move(other.m_Vertices)),
      m_Indices(std::move(other.m_Indices)),
      m_VAO(other.m_VAO), m_VBO(other.m_VBO), m_EBO(other.m_EBO)
{
    other.m_VAO = 0;
    other.m_VBO = 0;
    other.m_EBO = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other)
    {
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
        glDeleteVertexArrays(1, &m_VAO);

        m_Vertices = std::move(other.m_Vertices);
        m_Indices = std::move(other.m_Indices);
        m_VAO = other.m_VAO;
        m_VBO = other.m_VBO;
        m_EBO = other.m_EBO;
        other.m_VAO = 0;
        other.m_VBO = 0;
        other.m_EBO = 0;
    }
    return *this;
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices)
    : m_Vertices(std::move(vertices)), m_Indices(std::move(indices))
{
}

Mesh::Mesh(std::vector<Vertex> vertices)
    : m_Vertices(std::move(vertices)) {

}
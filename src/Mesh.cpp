//
// Created by smith on 13.07.26.
//

#include "headers/Mesh.h"

#include <iostream>

void CreateVertexArrays(GLuint &m_VAO) {
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
}

const void Mesh::Bind() {
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


void CreateNormalBuffer(GLuint &m_VBO, const std::vector<Vertex> &m_Vertices)
{
    glVertexAttribPointer(
    2,
    3,
    GL_FLOAT,
    GL_FALSE,
    sizeof(Vertex),
    reinterpret_cast<void*>(offsetof(Vertex, Normal)));

    glEnableVertexAttribArray(2);
}


void CreateMaterialIDBuffer(GLuint &m_VBO, const std::vector<Vertex> &m_Vertices)
{
    glVertexAttribPointer(
    3,
    1,
    GL_FLOAT,
    GL_FALSE,
    sizeof(Vertex),
    reinterpret_cast<void*>(offsetof(Vertex, MaterialID)));

    glEnableVertexAttribArray(3);
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
    CreateNormalBuffer(m_VBO, m_Vertices);
    CreateMaterialIDBuffer(m_VBO, m_Vertices);

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
    CreateBuffers(m_VBO, m_EBO, m_Data.vertices, m_Data.indices);
    glEnableVertexAttribArray(0);
}

void Mesh::UpdateData(std::vector<Vertex> vertices, std::vector<GLuint> indices)
{
    m_Data.vertices = std::move(vertices);
    m_Data.indices = std::move(indices);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Data.vertices.size() * sizeof(Vertex), m_Data.vertices.data(), GL_DYNAMIC_DRAW);

    if (m_EBO == 0) {
        glGenBuffers(1, &m_EBO);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Data.indices.size() * sizeof(GLuint), m_Data.indices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::Draw(const GLenum mode)
{
    glBindVertexArray(m_VAO);

    auto& [vertices, indices] = m_Data;

    if (!indices.empty())
        glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(mode, 0, vertices.size());
}

void Mesh::AttachInstanceBuffer(const InstanceBuffer & instance_buffer) {
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, instance_buffer.m_instance_VBO);

    // Starts at 4, not 2: locations 0-3 are always reserved for the
    // per-vertex Position/Color/Normal/MaterialID that CreateBuffers wires
    // up on every Mesh, so a per-instance mat4 (4 consecutive locations)
    // has to start past them to avoid overwriting those attribute pointers.
    for (int i = 0; i < 4; ++i)
    {
        const GLuint location = 4 + i;
        const auto offset = sizeof(glm::vec4) * i;

        glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE,
            sizeof(InstanceData), reinterpret_cast<void*>( offsetof(InstanceData, Transform) + offset));
        glEnableVertexAttribArray(location);
        glVertexAttribDivisor(location, 1);
    }
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_Data(std::move(other.m_Data)),
      m_VAO(other.m_VAO),
      m_VBO(other.m_VBO),
      m_EBO(other.m_EBO)
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
        m_Data = std::move(other.m_Data);
        m_VAO = other.m_VAO;
        m_VBO = other.m_VBO;
        m_EBO = other.m_EBO;
        other.m_VAO = 0;
        other.m_VBO = 0;
        other.m_EBO = 0;
    }
    return *this;
}

size_t Mesh::GetVertexCount() const {
    return m_Data.vertices.size();
}
size_t Mesh::GetIndexCount() const {
    return m_Data.indices.size();
}
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
    : m_Data{
        std::move(vertices),
        std::move(indices)
      }
{
}

Mesh::Mesh(std::vector<Vertex> vertices)
    : m_Data{
        std::move(vertices),
        {}
    }
{
}

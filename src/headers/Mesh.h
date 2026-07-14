//
// Created by smith on 13.07.26.
//

#ifndef OPENGLSKELETON_MESH_H
#define OPENGLSKELETON_MESH_H
#include <vector>
#include "glad/gl.h"
#include <glm/glm.hpp>

#include "InstanceBuffer.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Color;
};

class Mesh
{
public:
    explicit Mesh(std::vector<Vertex> vertices);
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);

    size_t GetIndexCount() const;
    size_t GetVertexCount() const;

    void Upload();
    void Draw();
    const void Bind();
    void AttachInstanceBuffer(const InstanceBuffer&);


    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;
private:
    std::vector<Vertex> m_Vertices;
    std::vector<GLuint> m_Indices;

    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    GLuint m_EBO = 0;
};


#endif //OPENGLSKELETON_MESH_H
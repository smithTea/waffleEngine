//
// Created by smith on 13.07.26.
//

#ifndef OPENGLSKELETON_MESH_H
#define OPENGLSKELETON_MESH_H
#include <vector>
#include "glad/gl.h"

struct Vertex
{
    float x;
    float y;
};

class Mesh
{
public:
    explicit Mesh(std::vector<Vertex> vertices);
    void Upload();
    void Draw();
private:
    std::vector<Vertex> m_Vertices;
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
};


#endif //OPENGLSKELETON_MESH_H
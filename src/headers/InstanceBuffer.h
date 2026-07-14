//
// Created by smith on 14.07.26.
//

#ifndef OPENGLSKELETON_INSTANCEBUFFER_H
#define OPENGLSKELETON_INSTANCEBUFFER_H
#include <vector>
#include <glm/fwd.hpp>

#include "glad/gl.h"
#include <glm/ext/matrix_transform.hpp>
struct InstanceData
{
    glm::mat4 Transform;
};

class InstanceBuffer {
    public:
    InstanceBuffer();
    friend class Mesh;
    friend class Renderer;
    void Upload();
    void Update();
    void GenerateMatrices(short x, short y, short z);
private:
    std::vector<InstanceData> m_data;
    GLuint m_instance_VBO;
};

#endif //OPENGLSKELETON_INSTANCEBUFFER_H
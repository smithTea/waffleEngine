//
// Created by smith on 14.07.26.
//

#ifndef OPENGLSKELETON_INSTANCEBUFFER_H
#define OPENGLSKELETON_INSTANCEBUFFER_H
#include <vector>
#include <glm/fwd.hpp>

#include "glad/gl.h"
#include <glm/ext/matrix_transform.hpp>

class InstanceBuffer {
    public:
    InstanceBuffer();
    void Upload();
    void GenerateMatrices(short x, short y, short z);
private:
    std::vector<glm::mat4> m_instance_matrices;
    GLuint m_instance_VBO;
};

#endif //OPENGLSKELETON_INSTANCEBUFFER_H
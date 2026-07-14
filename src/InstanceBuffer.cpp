//
// Created by smith on 14.07.26.
//

#include "headers/InstanceBuffer.h"

void InstanceBuffer::GenerateMatrices(const short x, const short y, const short z) {

    m_data.reserve(x * y * z);

    for(short i = 0; i < x; i++) {
       for(short j = 0; j < y; j++) {
           for (short k = 0; k < z; k++) {
               const glm::mat4 matrix = glm::translate(glm::mat4(1.0f), glm::vec3(i, j, k));
               m_data.push_back({ matrix });
           }
       }
    }
}

void CreateInstanceBuffer(GLuint &m_instance_VBO, const std::vector<InstanceData> &m_data) {
    if (m_instance_VBO == 0)
        glGenBuffers(1, &m_instance_VBO);

    glBindBuffer(GL_ARRAY_BUFFER, m_instance_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_data.size() * sizeof(InstanceData), m_data.data(), GL_STATIC_DRAW);
}

void InstanceBuffer::Upload() {
    CreateInstanceBuffer(m_instance_VBO, m_data);
}

void InstanceBuffer::Update() {
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_data.size() * sizeof(InstanceData), m_data.data(), GL_STATIC_DRAW);
}


InstanceBuffer::InstanceBuffer() : m_instance_VBO(0) {
}

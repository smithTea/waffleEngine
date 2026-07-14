//
// Created by smith on 14.07.26.
//

#include "headers/InstanceBuffer.h"

#include <format>


void InstanceBuffer::GenerateMatrices(const short x, const short y, const short z) {

    m_instance_matrices = {};

    for(short i = 0; i < x; i++) {
       for(short j = 0; j < y; j++) {
           for (short k = 0; k < z; k++) {
               glm::mat4 matrix = glm::translate(glm::mat4(1.0f), glm::vec3(i, j, k));
               m_instance_matrices.push_back(matrix);
           }
       }
    }
}

void InstanceBuffer::Upload() {

}

InstanceBuffer::InstanceBuffer() : m_instance_VBO(0) {
}

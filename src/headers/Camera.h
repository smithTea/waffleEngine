//
// Created by smith on 15.07.26.
//

#ifndef OPENGLSKELETON_CAMERA_H
#define OPENGLSKELETON_CAMERA_H

#include <cmath>
#include "../headers/Window.h"

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

using namespace std;

class Camera {
    public:
    Camera();
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(const Window& window);

    void ProcessCameraVectors();
    void ProcessKeyboardInput(const Window& window, float deltaTime);
    void ProcessMouseInput(float xOffset, float yOffset);

private:
    glm::vec3 m_Position{};
    glm::vec3 m_Front{};
    glm::vec3 m_Up{};
    glm::vec3 m_Right{};
    glm::vec3 m_WorldUp{0.0f, 1.0f, 0.0f};

    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
    float m_Fov = 45.0f;
    float m_BaseSpeed = 5.0f;
    float m_Speed;
    float m_SprintSpeedMulti = 3.0f;
    float m_Sensitivity = 0;
};


#endif //OPENGLSKELETON_CAMERA_H
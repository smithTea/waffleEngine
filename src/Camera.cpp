//
// Created by smith on 15.07.26.
//
#include "headers/Camera.h"

#include <algorithm>

void Camera::ProcessCameraVectors() {

    const auto yaw = glm::radians(m_yaw);
    const auto pitch = glm::radians(m_pitch);

    m_Front.x = cos(yaw) * cos(pitch);
    m_Front.y = sin(pitch);
    m_Front.z = sin(yaw) * cos(pitch);
    m_Front = glm::normalize(m_Front);

    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

glm::mat4 Camera::GetProjectionMatrix(const Window& window ) {
    return glm::perspective(glm::radians(m_Fov), window.GetAspectRatio(), 0.5f, 1000.0f);
}

void Camera::ProcessKeyboardInput(const Window& window, const float deltaTime)
{
    const float velocity = m_Speed * deltaTime;
    const auto w = window.GetNativeHandle();

    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS)
        m_Position += m_Front * velocity;

    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS)
        m_Position -= m_Front * velocity;

    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS)
        m_Position -= m_Right * velocity;

    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS)
        m_Position += m_Right * velocity;

    if (glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        m_Speed = m_BaseSpeed * m_SprintSpeedMulti;
    else
        m_Speed = m_BaseSpeed;

}

void Camera::ProcessMouseInput(const float xOffset, const float yOffset)
{
    constexpr float sensitivity = 0.1f;
    m_yaw += xOffset * sensitivity;
    m_pitch += yOffset * sensitivity;

    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    ProcessCameraVectors();
}

Camera::Camera() {
    m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_Speed = m_BaseSpeed;
}

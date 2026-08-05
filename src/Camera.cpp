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

    // Right is derived from yaw only (pitch left out), so it stays a
    // well-defined horizontal vector even when looking near-straight
    // up/down, instead of degenerating as m_Front nears m_WorldUp.
    const glm::vec3 frontYawOnly(cos(yaw), 0.0f, sin(yaw));
    m_Right = glm::normalize(glm::cross(frontYawOnly, m_WorldUp));
    m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

glm::mat4 Camera::GetProjectionMatrix(const Window& window ) {
    return glm::perspective(glm::radians(m_Fov), window.GetAspectRatio(), 0.05f, 1000.0f);
}

void Camera::ProcessKeyboardInput(const Window& window, const float deltaTime)
{
    // Minecraft creative-style flight. Gravity/collision-based walking
    // is a separate mode to add later; for now flight is the only way
    // to move, since that's what building needs.
    const auto w = window.GetNativeHandle();

    // Double-tap Space toggles flying on/off (edge-detected, with a
    // short window between presses).
    static bool spaceWasDown = false;
    static double lastSpacePressTime = -1.0;
    constexpr double doubleTapWindow = 0.3;

    const bool spaceDown = glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS;
    const bool spacePressed = spaceDown && !spaceWasDown;
    spaceWasDown = spaceDown;

    if (spacePressed) {
        const double now = glfwGetTime();
        if (lastSpacePressTime >= 0.0 && now - lastSpacePressTime <= doubleTapWindow) {
            m_Flying = !m_Flying;
            lastSpacePressTime = -1.0;
        } else {
            lastSpacePressTime = now;
        }
    }

    // Accelerate is Ctrl now that Shift is used to descend while flying.
    if (glfwGetKey(w, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        m_Speed = m_BaseSpeed * m_SprintSpeedMulti;
    else
        m_Speed = m_BaseSpeed;

    // WASD always moves horizontally along yaw only, not the pitched
    // look direction, matching Minecraft's flight controls.
    const glm::vec3 flatFront = glm::normalize(glm::vec3(m_Front.x, 0.0f, m_Front.z));

    glm::vec3 wishDir(0.0f);
    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) wishDir += flatFront;
    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) wishDir -= flatFront;
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) wishDir -= m_Right;
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) wishDir += m_Right;

    if (glm::length(wishDir) > 0.0f)
        wishDir = glm::normalize(wishDir);

    glm::vec3 targetVelocity = wishDir * m_Speed;

    if (m_Flying) {
        if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS)
            targetVelocity.y += m_Speed;
        if (glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            targetVelocity.y -= m_Speed;
    }

    // Ease actual velocity toward the target instead of snapping straight
    // to full speed/zero, so starting, stopping, and changing direction
    // have a bit of drift rather than feeling instant and rigid.
    constexpr float acceleration = 12.0f;
    const float lerpFactor = std::clamp(acceleration * deltaTime, 0.0f, 1.0f);
    m_Velocity = glm::mix(m_Velocity, targetVelocity, lerpFactor);

    m_Position += m_Velocity * deltaTime;
}

void Camera::ProcessMouseInput(const float xOffset, const float yOffset)
{
    constexpr float sensitivity = 0.1f;
    constexpr float pitchLimit = 89.0f;

    // Only damp yaw in the last stretch before the pitch clamp, where
    // the view is close enough to vertical for turning to visibly swirl
    // the world underneath. Normal look-around stays fully responsive.
    constexpr float dampStart = 75.0f;
    constexpr float minYawScale = 0.3f;

    const float pitchAbs = std::abs(m_pitch);
    float yawScale = 1.0f;
    if (pitchAbs > dampStart) {
        const float t = std::clamp((pitchAbs - dampStart) / (pitchLimit - dampStart), 0.0f, 1.0f);
        yawScale = glm::mix(1.0f, minYawScale, t);
    }

    m_yaw += xOffset * sensitivity * yawScale;
    m_pitch += yOffset * sensitivity;

    m_pitch = std::clamp(m_pitch, -pitchLimit, pitchLimit);

    ProcessCameraVectors();
}

Camera::Camera() {
    m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_Speed = m_BaseSpeed;
}

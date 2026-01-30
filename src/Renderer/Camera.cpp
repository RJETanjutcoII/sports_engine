// Camera.cpp
// Third-person camera with exponential smoothing for lag effect.
#include "Camera.hpp"
#include <algorithm>
#include <cmath>

namespace Sports {

Camera::Camera() {
    calculateTargetPosition();
    m_position = m_desiredPosition;
    m_smoothedTargetPos = m_targetPosition;
    updateViewMatrix();
    updateProjectionMatrix();
}

void Camera::setFollowTarget(const Vec3& targetPosition) {
    m_targetPosition = targetPosition;
}

void Camera::update(f32 deltaTime) {
    // Exponential interpolation: frame-rate independent smoothing
    // t = 1 - e^(-speed * dt) gives consistent smoothing at any framerate
    f32 t = 1.0f - std::exp(-m_lagSpeed * deltaTime);

    // Smoothly track the player position
    m_smoothedTargetPos.x += (m_targetPosition.x - m_smoothedTargetPos.x) * t;
    m_smoothedTargetPos.y += (m_targetPosition.y - m_smoothedTargetPos.y) * t;
    m_smoothedTargetPos.z += (m_targetPosition.z - m_smoothedTargetPos.z) * t;

    // Calculate camera position on sphere around target
    f32 horizontalDistance = m_followDistance * std::cos(m_pitch);
    f32 verticalOffset = m_followDistance * std::sin(m_pitch) + m_followHeight;

    // Offset from target based on yaw (horizontal angle)
    Vec3 offset(
        -horizontalDistance * std::sin(m_yaw),
        verticalOffset,
        horizontalDistance * std::cos(m_yaw)
    );

    m_position = m_smoothedTargetPos + offset;
    m_position.y = std::max(m_position.y, MIN_CAMERA_HEIGHT);  // Don't go underground

    updateViewMatrix();
}

void Camera::rotate(f32 deltaX, f32 deltaY) {
    m_yaw -= deltaX * m_sensitivity;
    m_pitch -= deltaY * m_sensitivity;

    // Clamp pitch to prevent flipping
    m_pitch = std::clamp(m_pitch, MIN_PITCH, MAX_PITCH);

    // Wrap yaw to avoid floating point drift
    const f32 TWO_PI = 6.28318530718f;
    if (m_yaw > TWO_PI) m_yaw -= TWO_PI;
    if (m_yaw < -TWO_PI) m_yaw += TWO_PI;
}

void Camera::zoom(f32 delta) {
    m_followDistance -= delta;
    m_followDistance = std::clamp(m_followDistance, MIN_DISTANCE, MAX_DISTANCE);
}

Vec3 Camera::getForwardXZ() const {
    // Forward direction projected onto XZ plane (for player movement)
    return glm::normalize(Vec3(
        std::sin(m_yaw),
        0.0f,
        -std::cos(m_yaw)
    ));
}

Vec3 Camera::getRightXZ() const {
    // Right direction projected onto XZ plane
    return glm::normalize(Vec3(
        std::cos(m_yaw),
        0.0f,
        std::sin(m_yaw)
    ));
}

void Camera::setPerspective(f32 fovDegrees, f32 aspectRatio, f32 nearPlane, f32 farPlane) {
    m_fov = fovDegrees;
    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    updateProjectionMatrix();
}

void Camera::setAspectRatio(f32 aspectRatio) {
    m_aspectRatio = aspectRatio;
    updateProjectionMatrix();
}

void Camera::calculateTargetPosition() {
    f32 horizontalDistance = m_followDistance * std::cos(m_pitch);
    f32 verticalOffset = m_followDistance * std::sin(m_pitch) + m_followHeight;

    Vec3 offset(
        -horizontalDistance * std::sin(m_yaw),
        verticalOffset,
        horizontalDistance * std::cos(m_yaw)
    );

    m_desiredPosition = m_targetPosition + offset;
    m_desiredPosition.y = std::max(m_desiredPosition.y, MIN_CAMERA_HEIGHT);
}

void Camera::updateViewMatrix() {
    // Look at a point slightly above target (player's head level)
    Vec3 lookAtPoint = m_smoothedTargetPos + Vec3(0.0f, 1.5f, 0.0f);

    m_viewMatrix = glm::lookAt(
        m_position,
        lookAtPoint,
        Vec3(0.0f, 1.0f, 0.0f)  // World up
    );
}

void Camera::updateProjectionMatrix() {
    m_projectionMatrix = glm::perspective(
        glm::radians(m_fov),
        m_aspectRatio,
        m_nearPlane,
        m_farPlane
    );
}

}

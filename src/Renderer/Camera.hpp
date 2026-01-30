// Camera.hpp
// Third-person follow camera with smooth interpolation.
#pragma once

#include "Core/Types.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Sports {

class Camera {
public:
    Camera();

    // Third-person controls
    void setFollowTarget(const Vec3& targetPosition);  // Who to follow
    void rotate(f32 deltaX, f32 deltaY);               // Mouse look
    void zoom(f32 delta);                               // Scroll wheel

    // Convert camera orientation to movement vectors (Y=0 for ground movement)
    Vec3 getForwardXZ() const;
    Vec3 getRightXZ() const;

    // Projection settings
    void setPerspective(f32 fovDegrees, f32 aspectRatio, f32 nearPlane, f32 farPlane);
    void setAspectRatio(f32 aspectRatio);

    // Matrix access for rendering
    const Mat4& getViewMatrix() const { return m_viewMatrix; }
    const Mat4& getProjectionMatrix() const { return m_projectionMatrix; }
    Mat4 getViewProjectionMatrix() const { return m_projectionMatrix * m_viewMatrix; }

    // State getters
    const Vec3& getPosition() const { return m_position; }
    f32 getYaw() const { return m_yaw; }
    f32 getPitch() const { return m_pitch; }

    // Tuning parameters
    void setFollowDistance(f32 distance) { m_followDistance = distance; }
    void setFollowHeight(f32 height) { m_followHeight = height; }
    void setSensitivity(f32 sensitivity) { m_sensitivity = sensitivity; }
    void setLagSpeed(f32 speed) { m_lagSpeed = speed; }

    void update(f32 deltaTime);

private:
    void calculateTargetPosition();
    void updateViewMatrix();
    void updateProjectionMatrix();

    Vec3 m_position{0.0f, 5.0f, 10.0f};       // Current camera position
    Vec3 m_desiredPosition{0.0f, 5.0f, 10.0f};
    Vec3 m_targetPosition{0.0f, 0.0f, 0.0f};  // Player position to follow
    Vec3 m_smoothedTargetPos{0.0f, 0.0f, 0.0f};

    f32 m_yaw = 0.0f;    // Horizontal rotation (radians)
    f32 m_pitch = 0.3f;  // Vertical angle (radians)

    // Pitch limits prevent camera from going underground or flipping
    static constexpr f32 MIN_PITCH = -0.5f;
    static constexpr f32 MAX_PITCH = 1.2f;

    f32 m_followDistance = 8.0f;   // Distance behind target
    f32 m_followHeight = 3.0f;     // Height above target
    f32 m_sensitivity = 0.002f;    // Mouse sensitivity
    f32 m_lagSpeed = 5.0f;         // How fast camera catches up

    static constexpr f32 MIN_DISTANCE = 3.0f;
    static constexpr f32 MAX_DISTANCE = 20.0f;
    static constexpr f32 MIN_CAMERA_HEIGHT = 1.0f;

    // Projection parameters
    f32 m_fov = 60.0f;
    f32 m_aspectRatio = 16.0f / 9.0f;
    f32 m_nearPlane = 0.1f;
    f32 m_farPlane = 500.0f;

    Mat4 m_viewMatrix{1.0f};
    Mat4 m_projectionMatrix{1.0f};
};

}

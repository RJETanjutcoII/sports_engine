// InputHandler.hpp
// SDL2 input processing and camera-relative movement calculation.
#pragma once

#include "Core/Types.hpp"
#include <SDL2/SDL.h>

namespace Sports {

class Camera;
class Window;

// Aggregates all player input for the current frame
struct InputState {
    Vec3 movementDirection{0.0f};   // Camera-relative XZ movement
    bool sprinting = false;
    bool kickPressed = false;
    bool kickJustPressed = false;   // True only on initial press frame
    f32 spinY = 0.0f;               // Curve direction from mouse buttons
};

class InputHandler {
public:
    InputHandler() = default;

    void processEvents(Window& window, Camera& camera);
    void updateKeyboardState(const Camera& camera);

    const InputState& getState() const { return m_state; }

    bool isMouseCaptured() const { return m_mouseCaptured; }
    void setMouseCaptured(bool captured);

    bool shouldResetBall() const { return m_resetBallRequested; }
    void clearResetBall() { m_resetBallRequested = false; }

    bool shouldToggleAI() const { return m_toggleAIRequested; }
    void clearToggleAI() { m_toggleAIRequested = false; }

private:
    void handleKeyDown(SDL_Keycode key, Window& window);
    void handleMouseMotion(i32 xrel, i32 yrel, Camera& camera);
    void handleMouseWheel(i32 y, Camera& camera);

    InputState m_state;
    bool m_mouseCaptured = true;
    bool m_prevKickPressed = false;
    bool m_resetBallRequested = false;
    bool m_toggleAIRequested = false;
};

}

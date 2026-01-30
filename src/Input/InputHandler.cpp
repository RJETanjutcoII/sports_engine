// InputHandler.cpp
// SDL2 event handling and camera-relative movement calculation.
#include "InputHandler.hpp"
#include "Core/Logger.hpp"
#include "Renderer/Window.hpp"
#include "Renderer/Camera.hpp"
#include <glad/gl.h>
#include <SDL2/SDL.h>

namespace Sports {

void InputHandler::processEvents(Window& window, Camera& camera) {
    // Reset one-shot flags each frame
    m_resetBallRequested = false;
    m_toggleAIRequested = false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                window.setShouldClose(true);
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    glViewport(0, 0, event.window.data1, event.window.data2);
                }
                break;

            case SDL_MOUSEMOTION:
                if (m_mouseCaptured) {
                    handleMouseMotion(event.motion.xrel, event.motion.yrel, camera);
                }
                break;

            case SDL_MOUSEWHEEL:
                handleMouseWheel(event.wheel.y, camera);
                break;

            case SDL_KEYDOWN:
                handleKeyDown(event.key.keysym.sym, window);
                break;
        }
    }
}

void InputHandler::updateKeyboardState(const Camera& camera) {
    const u8* keyState = SDL_GetKeyboardState(nullptr);

    // Convert WASD to camera-relative direction
    Vec3 forward = camera.getForwardXZ();
    Vec3 right = camera.getRightXZ();

    Vec3 inputDir(0.0f);
    if (keyState[SDL_SCANCODE_W]) inputDir += forward;
    if (keyState[SDL_SCANCODE_S]) inputDir -= forward;
    if (keyState[SDL_SCANCODE_D]) inputDir += right;
    if (keyState[SDL_SCANCODE_A]) inputDir -= right;

    if (glm::length(inputDir) > 0.01f) {
        inputDir = glm::normalize(inputDir);
    }

    m_state.movementDirection = inputDir;
    m_state.sprinting = keyState[SDL_SCANCODE_LSHIFT] != 0;

    // Edge detection for kick (only true on initial press)
    bool kickCurrentlyPressed = keyState[SDL_SCANCODE_SPACE] != 0;
    m_state.kickJustPressed = kickCurrentlyPressed && !m_prevKickPressed;
    m_state.kickPressed = kickCurrentlyPressed;
    m_prevKickPressed = kickCurrentlyPressed;

    // Mouse buttons apply spin to kicks
    u32 mouseButtons = SDL_GetMouseState(nullptr, nullptr);
    m_state.spinY = 0.0f;
    if (mouseButtons & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        m_state.spinY = 10.0f;   // Left click = curves right
    } else if (mouseButtons & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        m_state.spinY = -10.0f;  // Right click = curves left
    }
}

void InputHandler::setMouseCaptured(bool captured) {
    m_mouseCaptured = captured;
    SDL_SetRelativeMouseMode(captured ? SDL_TRUE : SDL_FALSE);
}

void InputHandler::handleKeyDown(SDL_Keycode key, Window& window) {
    switch (key) {
        case SDLK_ESCAPE:
            window.setShouldClose(true);
            break;

        case SDLK_TAB:
            setMouseCaptured(!m_mouseCaptured);
            LOG_INFO("Mouse capture: {}", m_mouseCaptured ? "ON" : "OFF");
            break;

        case SDLK_r:
            m_resetBallRequested = true;
            LOG_INFO("Ball reset requested");
            break;

        case SDLK_0:
            m_toggleAIRequested = true;
            break;
    }
}

void InputHandler::handleMouseMotion(i32 xrel, i32 yrel, Camera& camera) {
    // Invert X for natural camera rotation
    camera.rotate(
        -static_cast<f32>(xrel),
        static_cast<f32>(yrel)
    );
}

void InputHandler::handleMouseWheel(i32 y, Camera& camera) {
    camera.zoom(y * 0.5f);
}

}

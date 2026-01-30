// Window.hpp
// SDL2 window and OpenGL context management.
#pragma once

#include "Core/Types.hpp"
#include <string>
#include <SDL.h>

namespace Sports {

struct WindowConfig {
    std::string title = "Sports Engine";
    i32 width = 1280;
    i32 height = 720;
    bool vsync = true;
    bool fullscreen = false;
    bool resizable = true;
};

class Window {
public:
    Window();
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    bool init(const WindowConfig& config);
    void shutdown();
    void swapBuffers();
    void pollEvents();

    i32 getWidth() const { return m_width; }
    i32 getHeight() const { return m_height; }
    f32 getAspectRatio() const { return static_cast<f32>(m_width) / static_cast<f32>(m_height); }
    bool shouldClose() const { return m_shouldClose; }
    void setShouldClose(bool close) { m_shouldClose = close; }

    SDL_Window* getSDLWindow() const { return m_window; }

private:
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_glContext = nullptr;

    i32 m_width = 0;
    i32 m_height = 0;
    bool m_shouldClose = false;
};

}

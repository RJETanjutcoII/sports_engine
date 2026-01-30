// Window.cpp
// SDL2 initialization, OpenGL 4.5 context creation, and GLAD loading.
#include "Window.hpp"
#include "Core/Logger.hpp"

#include <glad/gl.h>

namespace Sports {

Window::Window() = default;

Window::~Window() {
    shutdown();
}

bool Window::init(const WindowConfig& config) {
    LOG_INFO("Initializing window: {}x{}", config.width, config.height);

    // SDL must be initialized before any window/input operations
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        LOG_ERROR("Failed to initialize SDL: {}", SDL_GetError());
        return false;
    }

    // Request OpenGL 4.5 core profile (no deprecated functions)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);    // 24-bit depth buffer
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);   // 8-bit stencil (unused but standard)
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);   // Double buffering for smooth rendering

    // Build window flags from config
    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (config.resizable) {
        windowFlags |= SDL_WINDOW_RESIZABLE;
    }
    if (config.fullscreen) {
        windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    m_window = SDL_CreateWindow(
        config.title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config.width,
        config.height,
        windowFlags
    );

    if (!m_window) {
        LOG_ERROR("Failed to create window: {}", SDL_GetError());
        return false;
    }

    m_width = config.width;
    m_height = config.height;

    // OpenGL context must be created after window
    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        LOG_ERROR("Failed to create OpenGL context: {}", SDL_GetError());
        return false;
    }

    SDL_GL_MakeCurrent(m_window, m_glContext);

    // GLAD loads OpenGL function pointers at runtime
    int gladVersion = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
    if (!gladVersion) {
        LOG_ERROR("Failed to initialize GLAD");
        return false;
    }

    LOG_INFO("OpenGL loaded successfully");
    LOG_INFO("  Vendor: {}", (const char*)glGetString(GL_VENDOR));
    LOG_INFO("  Renderer: {}", (const char*)glGetString(GL_RENDERER));
    LOG_INFO("  Version: {}", (const char*)glGetString(GL_VERSION));

    // VSync: 1 = synchronized, 0 = unlimited FPS
    SDL_GL_SetSwapInterval(config.vsync ? 1 : 0);

    // Default GL state
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.4f, 0.1f, 1.0f);  // Dark green (grass-like)

    LOG_INFO("Window initialized successfully");
    return true;
}

void Window::shutdown() {
    if (m_glContext) {
        SDL_GL_DeleteContext(m_glContext);
        m_glContext = nullptr;
    }

    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }

    SDL_Quit();
    LOG_INFO("Window shutdown complete");
}

void Window::swapBuffers() {
    // Present rendered frame to screen
    SDL_GL_SwapWindow(m_window);
}

void Window::pollEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                m_shouldClose = true;
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    m_width = event.window.data1;
                    m_height = event.window.data2;
                    // Update viewport to match new window size
                    glViewport(0, 0, m_width, m_height);
                    LOG_DEBUG("Window resized to {}x{}", m_width, m_height);
                }
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    m_shouldClose = true;
                }
                break;
        }
    }
}

}

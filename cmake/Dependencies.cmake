include(FetchContent)

# GLM - Mathematics library
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)

# EnTT - ECS library
FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.13.2
    GIT_SHALLOW TRUE
)

# SDL2 - Cross-platform multimedia library
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.0
    GIT_SHALLOW TRUE
)

# spdlog - Fast logging library
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.14.1
    GIT_SHALLOW TRUE
)

# GLAD - OpenGL loader (we'll generate and include it locally)
# For now, we'll download a pre-configured version
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.6
    GIT_SHALLOW TRUE
)

# Google Test (for testing)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    GIT_SHALLOW TRUE
)

# Make dependencies available
message(STATUS "Fetching dependencies...")

FetchContent_MakeAvailable(glm)
message(STATUS "GLM fetched")

FetchContent_MakeAvailable(entt)
message(STATUS "EnTT fetched")

# SDL2 configuration
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL2)
message(STATUS "SDL2 fetched")

FetchContent_MakeAvailable(spdlog)
message(STATUS "spdlog fetched")

# GLAD configuration - using glad2 cmake integration
FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    FetchContent_Populate(glad)

    # Add glad as a library
    add_subdirectory(${glad_SOURCE_DIR}/cmake ${glad_BINARY_DIR})
    glad_add_library(glad REPRODUCIBLE API gl:core=4.5)
endif()
message(STATUS "GLAD configured")

if(SPORTS_ENGINE_BUILD_TESTS)
    FetchContent_MakeAvailable(googletest)
    message(STATUS "GoogleTest fetched")
endif()

message(STATUS "All dependencies ready")

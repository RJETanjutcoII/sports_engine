cmake_minimum_required(VERSION 3.21)
project(SportsEngine
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Soccer Game Engine"
)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(SPORTS_ENGINE_BUILD_TESTS "Build unit tests" ON)

# Include helper modules
include(cmake/Dependencies.cmake)

# Collect all source files
file(GLOB_RECURSE ENGINE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE ENGINE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    glm::glm
    EnTT::EnTT
    SDL2::SDL2
    SDL2::SDL2main
    spdlog::spdlog
    glad
)

# Copy assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# Testing
if(SPORTS_ENGINE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

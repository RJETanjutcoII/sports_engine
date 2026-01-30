# Sports Engine

A 3D soccer game built from scratch in C++ using OpenGL 4.5.

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-4.5-green.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)

## Features

- **Third-person camera** with smooth follow and mouse-look controls
- **Ball physics** including gravity, drag, Magnus effect (spin curves), bounce, and rolling friction
- **AI opponents** with state machine behavior and team coordination
- **Player controls** with sprinting, dribbling, and spin kicks
- **Goal detection** with celebration animations
- **Procedural geometry** for all game objects (no external models required)

## Controls

| Key | Action |
|-----|--------|
| WASD | Move player |
| Shift | Sprint |
| Space | Kick ball |
| Left Click + Space | Kick with left spin (curves right) |
| Right Click + Space | Kick with right spin (curves left) |
| Mouse | Look around |
| Scroll | Zoom in/out |
| Tab | Toggle mouse capture |
| R | Reset ball |
| Escape | Quit |

## Building

### Requirements

- CMake 3.20+
- C++17 compatible compiler (MSVC, GCC, Clang)
- Windows (SDL2 and other dependencies are fetched automatically)

### Build Steps

```bash
# Configure
cmake -B build -S .

# Build
cmake --build build --config Release

# Or use the provided batch file on Windows
Build.bat
```

The executable will be in `build/Release/SportsEngine.exe`.

## Project Structure

```
sports_engine/
├── src/
│   ├── Core/           # Types, logging, timing utilities
│   ├── Game/           # Ball, Player, AI, Match logic
│   ├── Input/          # SDL2 input handling
│   ├── Physics/        # Ball physics simulation
│   ├── Renderer/       # Window, Shader, Camera, Mesh, Primitives
│   └── main.cpp        # Application entry point
├── assets/
│   └── shaders/        # GLSL vertex and fragment shaders
├── cmake/              # CMake modules
└── CMakeLists.txt
```

## Technical Highlights

- **Frame-rate independent physics** using delta time
- **Exponential smoothing** for camera lag (`1 - e^(-speed * dt)`)
- **Magnus effect** simulates realistic ball spin and curve
- **Uniform caching** in shader class to minimize GL calls
- **Move semantics** for GPU resource management

## Dependencies

All dependencies are fetched automatically via CMake FetchContent:

- [SDL2](https://www.libsdl.org/) - Window and input
- [GLAD](https://glad.dav1d.de/) - OpenGL loader
- [GLM](https://github.com/g-truc/glm) - Math library
- [spdlog](https://github.com/gabime/spdlog) - Logging

## License

MIT License

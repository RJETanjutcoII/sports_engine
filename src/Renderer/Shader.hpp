// Shader.hpp
// OpenGL shader program wrapper with uniform caching.
#pragma once

#include "Core/Types.hpp"
#include <string>
#include <unordered_map>

namespace Sports {

class Shader {
public:
    Shader();
    ~Shader();

    // Non-copyable (GPU resource)
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Move semantics for resource transfer
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    // Load from GLSL source strings or file paths
    bool loadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);

    void bind() const;    // Activate this shader for rendering
    void unbind() const;  // Deactivate (use default pipeline)

    bool isValid() const { return m_programID != 0; }

    // Uniform setters (upload data to GPU)
    void setInt(const std::string& name, i32 value);
    void setFloat(const std::string& name, f32 value);
    void setVec2(const std::string& name, const Vec2& value);
    void setVec3(const std::string& name, const Vec3& value);
    void setVec4(const std::string& name, const Vec4& value);
    void setMat3(const std::string& name, const Mat3& value);
    void setMat4(const std::string& name, const Mat4& value);

private:
    u32 m_programID = 0;
    mutable std::unordered_map<std::string, i32> m_uniformCache;  // Avoids repeated lookups

    u32 compileShader(u32 type, const std::string& source);
    i32 getUniformLocation(const std::string& name) const;
    std::string readFile(const std::string& path);
};

}

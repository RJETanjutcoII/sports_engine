// Shader.cpp
// GLSL compilation, linking, and uniform upload.
#include "Shader.hpp"
#include "Core/Logger.hpp"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>

namespace Sports {

Shader::Shader() = default;

Shader::~Shader() {
    if (m_programID != 0) {
        glDeleteProgram(m_programID);
    }
}

Shader::Shader(Shader&& other) noexcept
    : m_programID(other.m_programID)
    , m_uniformCache(std::move(other.m_uniformCache)) {
    other.m_programID = 0;  // Prevent double-delete
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (m_programID != 0) {
            glDeleteProgram(m_programID);
        }
        m_programID = other.m_programID;
        m_uniformCache = std::move(other.m_uniformCache);
        other.m_programID = 0;
    }
    return *this;
}

bool Shader::loadFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
    // Compile individual shader stages
    u32 vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    if (vertexShader == 0) {
        return false;
    }

    u32 fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    // Link shaders into a program
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertexShader);
    glAttachShader(m_programID, fragmentShader);
    glLinkProgram(m_programID);

    i32 success;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_programID, 512, nullptr, infoLog);
        LOG_ERROR("Shader program linking failed: {}", infoLog);
        glDeleteProgram(m_programID);
        m_programID = 0;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    // Shaders are now baked into program, originals can be deleted
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    LOG_DEBUG("Shader program created successfully (ID: {})", m_programID);
    return true;
}

bool Shader::loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexSource = readFile(vertexPath);
    if (vertexSource.empty()) {
        LOG_ERROR("Failed to read vertex shader: {}", vertexPath);
        return false;
    }

    std::string fragmentSource = readFile(fragmentPath);
    if (fragmentSource.empty()) {
        LOG_ERROR("Failed to read fragment shader: {}", fragmentPath);
        return false;
    }

    return loadFromSource(vertexSource, fragmentSource);
}

void Shader::bind() const {
    glUseProgram(m_programID);
}

void Shader::unbind() const {
    glUseProgram(0);
}

void Shader::setInt(const std::string& name, i32 value) {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, f32 value) {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, const Vec2& value) {
    glUniform2f(getUniformLocation(name), value.x, value.y);
}

void Shader::setVec3(const std::string& name, const Vec3& value) {
    glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void Shader::setVec4(const std::string& name, const Vec4& value) {
    glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setMat3(const std::string& name, const Mat3& value) {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const Mat4& value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

u32 Shader::compileShader(u32 type, const std::string& source) {
    u32 shader = glCreateShader(type);

    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    i32 success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        const char* typeName = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
        LOG_ERROR("{} shader compilation failed: {}", typeName, infoLog);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

i32 Shader::getUniformLocation(const std::string& name) const {
    // Check cache first to avoid expensive GL calls
    auto it = m_uniformCache.find(name);
    if (it != m_uniformCache.end()) {
        return it->second;
    }

    i32 location = glGetUniformLocation(m_programID, name.c_str());
    if (location == -1) {
        LOG_WARN("Uniform '{}' not found in shader", name);
    }

    m_uniformCache[name] = location;
    return location;
}

std::string Shader::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

}

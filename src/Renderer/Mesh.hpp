// Mesh.hpp
// OpenGL VAO/VBO/EBO wrapper for vertex data.
#pragma once

#include "Core/Types.hpp"
#include <vector>

namespace Sports {

// Interleaved vertex format: position + normal + color
struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec3 color;

    Vertex() = default;

    Vertex(const Vec3& pos, const Vec3& norm, const Vec3& col)
        : position(pos), normal(norm), color(col) {}

    // Convenience: auto-generate upward normal
    Vertex(const Vec3& pos, const Vec3& col)
        : position(pos), normal(0.0f, 1.0f, 0.0f), color(col) {}
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    // Non-copyable (GPU resource)
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Move semantics for resource transfer
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    // Upload vertex data to GPU (optional indices for indexed drawing)
    void upload(const std::vector<Vertex>& vertices,
                const std::vector<u32>& indices = {});

    void draw() const;

    bool isValid() const { return m_vao != 0; }

    u32 getVertexCount() const { return m_vertexCount; }
    u32 getIndexCount() const { return m_indexCount; }

private:
    void cleanup();

    u32 m_vao = 0;          // Vertex Array Object (stores vertex format)
    u32 m_vbo = 0;          // Vertex Buffer Object (vertex data)
    u32 m_ebo = 0;          // Element Buffer Object (indices)
    u32 m_vertexCount = 0;
    u32 m_indexCount = 0;
    bool m_useIndices = false;
};

}

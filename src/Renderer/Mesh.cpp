// Mesh.cpp
// VAO setup and indexed/non-indexed draw calls.
#include "Mesh.hpp"
#include "Core/Logger.hpp"

#include <glad/gl.h>

namespace Sports {

Mesh::Mesh() = default;

Mesh::~Mesh() {
    cleanup();
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_vao(other.m_vao)
    , m_vbo(other.m_vbo)
    , m_ebo(other.m_ebo)
    , m_vertexCount(other.m_vertexCount)
    , m_indexCount(other.m_indexCount)
    , m_useIndices(other.m_useIndices) {
    // Clear source to prevent double-free
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        cleanup();
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;
        m_vertexCount = other.m_vertexCount;
        m_indexCount = other.m_indexCount;
        m_useIndices = other.m_useIndices;
        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;
    }
    return *this;
}

void Mesh::upload(const std::vector<Vertex>& vertices, const std::vector<u32>& indices) {
    cleanup();

    if (vertices.empty()) {
        LOG_WARN("Attempted to upload empty mesh");
        return;
    }

    m_vertexCount = static_cast<u32>(vertices.size());
    m_indexCount = static_cast<u32>(indices.size());
    m_useIndices = !indices.empty();

    // VAO stores vertex attribute configuration
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // VBO stores actual vertex data
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW  // Data won't change after upload
    );

    // Configure vertex attributes (matches shader layout locations)
    // Location 0: position (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Location 1: normal (vec3)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Location 2: color (vec3)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    // EBO allows vertex reuse via indices
    if (m_useIndices) {
        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof(u32),
            indices.data(),
            GL_STATIC_DRAW
        );
    }

    glBindVertexArray(0);

    LOG_DEBUG("Mesh uploaded: {} vertices, {} indices", m_vertexCount, m_indexCount);
}

void Mesh::draw() const {
    if (m_vao == 0) return;

    glBindVertexArray(m_vao);

    if (m_useIndices) {
        // Indexed draw: uses EBO to specify triangle vertices
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    } else {
        // Non-indexed: every 3 vertices = 1 triangle
        glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    }

    glBindVertexArray(0);
}

void Mesh::cleanup() {
    // Delete in reverse order of creation
    if (m_ebo != 0) {
        glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
    }
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}

}

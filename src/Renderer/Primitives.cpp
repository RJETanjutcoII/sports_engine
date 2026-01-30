// Primitives.cpp
// Vertex and index generation for procedural geometry.
#include "Primitives.hpp"
#include <cmath>

namespace Sports::Primitives {

constexpr f32 PI = 3.14159265358979323846f;
constexpr f32 TWO_PI = PI * 2.0f;

MeshData createPlane(f32 width, f32 height, const Vec3& color, u32 segments) {
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    f32 halfWidth = width / 2.0f;
    f32 halfHeight = height / 2.0f;

    Vec3 normal(0.0f, 1.0f, 0.0f);  // Facing up

    // Create grid of vertices for potential tessellation
    for (u32 z = 0; z <= segments; ++z) {
        for (u32 x = 0; x <= segments; ++x) {
            f32 xPos = -halfWidth + (width * x / segments);
            f32 zPos = -halfHeight + (height * z / segments);
            vertices.emplace_back(Vec3(xPos, 0.0f, zPos), normal, color);
        }
    }

    // Generate triangle indices (two triangles per grid cell)
    for (u32 z = 0; z < segments; ++z) {
        for (u32 x = 0; x < segments; ++x) {
            u32 topLeft = z * (segments + 1) + x;
            u32 topRight = topLeft + 1;
            u32 bottomLeft = (z + 1) * (segments + 1) + x;
            u32 bottomRight = bottomLeft + 1;

            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    return {vertices, indices};
}

MeshData createSphere(f32 radius, const Vec3& color, u32 rings, u32 sectors) {
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    // Generate vertices using spherical coordinates
    for (u32 r = 0; r <= rings; ++r) {
        f32 phi = PI * r / rings;  // 0 to PI (top to bottom)
        f32 sinPhi = std::sin(phi);
        f32 cosPhi = std::cos(phi);

        for (u32 s = 0; s <= sectors; ++s) {
            f32 theta = TWO_PI * s / sectors;  // 0 to 2PI (around)
            f32 sinTheta = std::sin(theta);
            f32 cosTheta = std::cos(theta);

            // Normal = normalized position for unit sphere
            Vec3 normal(sinPhi * cosTheta, cosPhi, sinPhi * sinTheta);
            Vec3 position = normal * radius;

            vertices.emplace_back(position, normal, color);
        }
    }

    // Generate indices (quads split into triangles)
    for (u32 r = 0; r < rings; ++r) {
        for (u32 s = 0; s < sectors; ++s) {
            u32 current = r * (sectors + 1) + s;
            u32 next = current + sectors + 1;

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    return {vertices, indices};
}

MeshData createCapsule(f32 radius, f32 height, const Vec3& color, u32 rings, u32 sectors) {
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    // Capsule = cylinder + two hemispherical caps
    f32 cylinderHeight = height - 2 * radius;
    if (cylinderHeight < 0) cylinderHeight = 0;

    f32 halfCylinder = cylinderHeight / 2.0f;

    // Top hemisphere (phi: 0 to PI/2)
    for (u32 r = 0; r <= rings / 2; ++r) {
        f32 phi = (PI / 2.0f) * r / (rings / 2);
        f32 sinPhi = std::sin(phi);
        f32 cosPhi = std::cos(phi);

        for (u32 s = 0; s <= sectors; ++s) {
            f32 theta = TWO_PI * s / sectors;
            f32 sinTheta = std::sin(theta);
            f32 cosTheta = std::cos(theta);

            Vec3 normal(sinPhi * cosTheta, cosPhi, sinPhi * sinTheta);
            Vec3 position = normal * radius;
            position.y += halfCylinder;  // Shift up

            vertices.emplace_back(position, normal, color);
        }
    }

    u32 topHemiVerts = static_cast<u32>(vertices.size());

    // Cylinder body (just top and bottom rings)
    for (u32 h = 0; h <= 1; ++h) {
        f32 y = halfCylinder - cylinderHeight * h;

        for (u32 s = 0; s <= sectors; ++s) {
            f32 theta = TWO_PI * s / sectors;
            f32 sinTheta = std::sin(theta);
            f32 cosTheta = std::cos(theta);

            Vec3 normal(cosTheta, 0.0f, sinTheta);  // Points outward
            Vec3 position(radius * cosTheta, y, radius * sinTheta);

            vertices.emplace_back(position, normal, color);
        }
    }

    u32 cylinderVerts = static_cast<u32>(vertices.size());

    // Bottom hemisphere (phi: PI/2 to PI)
    for (u32 r = rings / 2; r <= rings; ++r) {
        f32 phi = PI / 2.0f + (PI / 2.0f) * (r - rings / 2) / (rings / 2);
        f32 sinPhi = std::sin(phi);
        f32 cosPhi = std::cos(phi);

        for (u32 s = 0; s <= sectors; ++s) {
            f32 theta = TWO_PI * s / sectors;
            f32 sinTheta = std::sin(theta);
            f32 cosTheta = std::cos(theta);

            Vec3 normal(sinPhi * cosTheta, cosPhi, sinPhi * sinTheta);
            Vec3 position = normal * radius;
            position.y -= halfCylinder;  // Shift down

            vertices.emplace_back(position, normal, color);
        }
    }

    // Top hemisphere indices
    for (u32 r = 0; r < rings / 2; ++r) {
        for (u32 s = 0; s < sectors; ++s) {
            u32 current = r * (sectors + 1) + s;
            u32 next = current + sectors + 1;

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    // Cylinder indices
    u32 cylStart = topHemiVerts;
    for (u32 s = 0; s < sectors; ++s) {
        u32 current = cylStart + s;
        u32 next = current + sectors + 1;

        indices.push_back(current);
        indices.push_back(next);
        indices.push_back(current + 1);

        indices.push_back(current + 1);
        indices.push_back(next);
        indices.push_back(next + 1);
    }

    // Bottom hemisphere indices
    u32 bottomStart = cylinderVerts;
    for (u32 r = 0; r < rings / 2; ++r) {
        for (u32 s = 0; s < sectors; ++s) {
            u32 current = bottomStart + r * (sectors + 1) + s;
            u32 next = current + sectors + 1;

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    return {vertices, indices};
}

MeshData createCube(f32 size, const Vec3& color) {
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    f32 h = size / 2.0f;

    // 8 corner positions
    Vec3 corners[8] = {
        {-h, -h, -h}, { h, -h, -h}, { h,  h, -h}, {-h,  h, -h},
        {-h, -h,  h}, { h, -h,  h}, { h,  h,  h}, {-h,  h,  h}
    };

    // 6 faces with their vertex indices and outward normals
    struct Face {
        u32 v0, v1, v2, v3;
        Vec3 normal;
    };

    Face faces[6] = {
        {0, 1, 2, 3, { 0,  0, -1}},  // Back
        {4, 7, 6, 5, { 0,  0,  1}},  // Front
        {0, 3, 7, 4, {-1,  0,  0}},  // Left
        {1, 5, 6, 2, { 1,  0,  0}},  // Right
        {3, 2, 6, 7, { 0,  1,  0}},  // Top
        {0, 4, 5, 1, { 0, -1,  0}}   // Bottom
    };

    // Each face needs separate vertices (flat shading)
    for (const auto& face : faces) {
        u32 startIndex = static_cast<u32>(vertices.size());

        vertices.emplace_back(corners[face.v0], face.normal, color);
        vertices.emplace_back(corners[face.v1], face.normal, color);
        vertices.emplace_back(corners[face.v2], face.normal, color);
        vertices.emplace_back(corners[face.v3], face.normal, color);

        // Two triangles per quad
        indices.push_back(startIndex);
        indices.push_back(startIndex + 1);
        indices.push_back(startIndex + 2);

        indices.push_back(startIndex);
        indices.push_back(startIndex + 2);
        indices.push_back(startIndex + 3);
    }

    return {vertices, indices};
}

MeshData createLine(const Vec3& start, const Vec3& end, f32 width, const Vec3& color) {
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    Vec3 direction = end - start;
    Vec3 up(0.0f, 1.0f, 0.0f);

    // Perpendicular to line direction (for width)
    Vec3 right = glm::normalize(glm::cross(direction, up)) * (width / 2.0f);
    Vec3 heightOffset(0.0f, 0.01f, 0.0f);  // Slight Y offset to prevent z-fighting
    Vec3 normal(0.0f, 1.0f, 0.0f);

    // Quad corners
    vertices.emplace_back(start - right + heightOffset, normal, color);
    vertices.emplace_back(start + right + heightOffset, normal, color);
    vertices.emplace_back(end + right + heightOffset, normal, color);
    vertices.emplace_back(end - right + heightOffset, normal, color);

    indices = {0, 1, 2, 0, 2, 3};

    return {vertices, indices};
}

MeshData createCone(f32 radius, f32 height, const Vec3& color, u32 sectors) {
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    Vec3 tip(0.0f, 0.0f, height);
    Vec3 tipNormal(0.0f, 0.0f, 1.0f);

    Vec3 baseCenter(0.0f, 0.0f, 0.0f);
    Vec3 baseNormal(0.0f, 0.0f, -1.0f);

    // Generate ring of vertices at base
    for (u32 i = 0; i <= sectors; ++i) {
        f32 theta = TWO_PI * i / sectors;
        f32 x = radius * std::cos(theta);
        f32 y = radius * std::sin(theta);

        Vec3 basePoint(x, y, 0.0f);
        // Side normal: blend between outward and upward
        Vec3 sideNormal = glm::normalize(Vec3(x, y, radius / height));

        // Two vertices per position: one for base, one for side
        vertices.emplace_back(basePoint, baseNormal, color);
        vertices.emplace_back(basePoint, sideNormal, color);
    }

    u32 tipIndex = static_cast<u32>(vertices.size());
    vertices.emplace_back(tip, tipNormal, color);

    u32 baseCenterIndex = static_cast<u32>(vertices.size());
    vertices.emplace_back(baseCenter, baseNormal, color);

    // Cone side triangles (connect base ring to tip)
    for (u32 i = 0; i < sectors; ++i) {
        u32 current = i * 2 + 1;  // Side vertex
        u32 next = ((i + 1) % (sectors + 1)) * 2 + 1;

        indices.push_back(current);
        indices.push_back(next);
        indices.push_back(tipIndex);
    }

    // Base cap triangles (fan from center)
    for (u32 i = 0; i < sectors; ++i) {
        u32 current = i * 2;  // Base vertex
        u32 next = ((i + 1) % (sectors + 1)) * 2;

        indices.push_back(baseCenterIndex);
        indices.push_back(next);
        indices.push_back(current);
    }

    return {vertices, indices};
}

}

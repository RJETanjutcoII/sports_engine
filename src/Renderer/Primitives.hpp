// Primitives.hpp
// Procedural mesh generation for common 3D shapes.
#pragma once

#include "Mesh.hpp"
#include <utility>

namespace Sports {

// Pair of vertices + indices ready for Mesh::upload()
using MeshData = std::pair<std::vector<Vertex>, std::vector<u32>>;

namespace Primitives {

// Flat horizontal plane (for field, ground, etc.)
MeshData createPlane(f32 width, f32 height, const Vec3& color, u32 segments = 1);

// UV sphere (for ball)
MeshData createSphere(f32 radius, const Vec3& color, u32 rings = 16, u32 sectors = 32);

// Cylinder with hemisphere caps (for player bodies)
MeshData createCapsule(f32 radius, f32 height, const Vec3& color, u32 rings = 8, u32 sectors = 16);

// Axis-aligned box
MeshData createCube(f32 size, const Vec3& color);

// Thin quad between two points (for field markings)
MeshData createLine(const Vec3& start, const Vec3& end, f32 width, const Vec3& color);

// Pointed cone (for player face direction indicator)
MeshData createCone(f32 radius, f32 height, const Vec3& color, u32 sectors = 16);

}

}

// Types.hpp
// Engine-wide type aliases for consistent sizing and GLM math types.
#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Sports {

// Fixed-width integer types for platform consistency
using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

// Floating Point Types
using f32 = float;
using f64 = double;

// GLM Vector and Matrix Types
using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;

using Mat3 = glm::mat3;
using Mat4 = glm::mat4;

using Quat = glm::quat;

// Entity ID Type
using EntityID = u32;
constexpr EntityID NULL_ENTITY = 0;

}

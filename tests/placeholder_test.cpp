// =============================================================================
// placeholder_test.cpp - Placeholder Test
// =============================================================================
// This is a simple test to verify the testing infrastructure works.
// We'll add real tests for physics and collision detection later.
// =============================================================================

#include <gtest/gtest.h>
#include <glm/glm.hpp>

// Simple test to verify GLM works
TEST(MathTest, VectorAddition) {
    glm::vec3 a(1.0f, 2.0f, 3.0f);
    glm::vec3 b(4.0f, 5.0f, 6.0f);
    glm::vec3 result = a + b;

    EXPECT_FLOAT_EQ(result.x, 5.0f);
    EXPECT_FLOAT_EQ(result.y, 7.0f);
    EXPECT_FLOAT_EQ(result.z, 9.0f);
}

TEST(MathTest, VectorDotProduct) {
    glm::vec3 a(1.0f, 0.0f, 0.0f);
    glm::vec3 b(0.0f, 1.0f, 0.0f);

    // Perpendicular vectors have dot product of 0
    EXPECT_FLOAT_EQ(glm::dot(a, b), 0.0f);

    // Parallel vectors
    glm::vec3 c(1.0f, 0.0f, 0.0f);
    EXPECT_FLOAT_EQ(glm::dot(a, c), 1.0f);
}

TEST(MathTest, VectorNormalization) {
    glm::vec3 v(3.0f, 4.0f, 0.0f);
    glm::vec3 normalized = glm::normalize(v);

    // Length should be 1
    EXPECT_NEAR(glm::length(normalized), 1.0f, 0.0001f);
}

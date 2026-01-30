#version 450 core
// =============================================================================
// basic.vert - Vertex Shader
// =============================================================================
// This shader runs ONCE FOR EACH VERTEX in your mesh.
// Its job: Transform vertex positions from model space to screen space.
//
// GLSL (OpenGL Shading Language) looks like C but runs on the GPU.
// =============================================================================

// Input Attributes - These match glVertexAttribPointer indices in Mesh.cpp
layout(location = 0) in vec3 aPosition;  // Vertex position
layout(location = 1) in vec3 aNormal;    // Vertex normal
layout(location = 2) in vec3 aColor;     // Vertex color

// Output to Fragment Shader - interpolated across the triangle
out vec3 vPosition;  // World position (for lighting)
out vec3 vNormal;    // Normal vector (for lighting)
out vec3 vColor;     // Color

// Uniforms - values set from C++ that stay constant per draw call
uniform mat4 uModel;       // Model matrix: object -> world space
uniform mat4 uView;        // View matrix: world -> camera space
uniform mat4 uProjection;  // Projection matrix: camera -> screen space

void main() {
    // Transform position to world space
    vec4 worldPosition = uModel * vec4(aPosition, 1.0);
    vPosition = worldPosition.xyz;

    // Transform normal to world space
    // transpose(inverse()) handles non-uniform scaling correctly
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;

    // Pass color through unchanged
    vColor = aColor;

    // Final clip-space position for rasterization
    gl_Position = uProjection * uView * worldPosition;
}

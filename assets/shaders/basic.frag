#version 450 core
// =============================================================================
// basic.frag - Fragment Shader
// =============================================================================
// This shader runs ONCE FOR EACH PIXEL that the triangle covers.
// Its job: Determine the final color of each pixel.
// =============================================================================

// Input from Vertex Shader (interpolated values)
in vec3 vPosition;  // World position
in vec3 vNormal;    // Normal vector
in vec3 vColor;     // Color

// Output - the final pixel color
out vec4 FragColor;

// Uniforms for lighting
uniform vec3 uLightDir;       // Direction TO the light (normalized)
uniform vec3 uLightColor;     // Light color (usually white)
uniform vec3 uAmbientColor;   // Ambient light (fills in shadows)
uniform vec3 uCameraPos;      // Camera position (for specular)

void main() {
    // Normalize the normal (interpolation can de-normalize it)
    vec3 normal = normalize(vNormal);

    // Ambient Lighting - constant light everywhere
    vec3 ambient = uAmbientColor * vColor;

    // Diffuse Lighting - surfaces facing light are brighter
    float diff = max(dot(normal, uLightDir), 0.0);
    vec3 diffuse = diff * uLightColor * vColor;

    // Specular Lighting - shiny highlights
    vec3 viewDir = normalize(uCameraPos - vPosition);
    vec3 halfDir = normalize(uLightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), 32.0);
    vec3 specular = spec * uLightColor * 0.3;

    // Combine all lighting
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}

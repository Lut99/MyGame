/* VERTEX SHADER
 *   by Lut99
 *
 * Shader for mapping the vertices from world space to the output frame space.
 */

#version 450

// Buffer for storing our triangle; not really pretty, but very easy
vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

// Buffer for storing the colours of our triangle; not really pretty, but very easy
vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

// We specify an output to the first framebuffer s.t. we can pass the colors to the fragment shader
layout(location = 0) out vec3 fragColor;

// Entry point for the shader
void main() {
    // For now, just quickly return the global positions as vector
    // The gl_Position is the global for the output position in frame space,
    //   and gl_VertexIndex specifies which vertex we're currently working on.
    //   Note that this means that the program won't work for more than three
    //   shaders!
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    
    // Also pass the colour on
    fragColor = colors[gl_VertexIndex];
}

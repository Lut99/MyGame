/* VERTEX SHADER
 *   by Lut99
 *
 * Shader for mapping the vertices from world space to the output frame space.
 */

#version 450

// Specify the input we use to get the vertex position
layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec3 vertex_color;

// We specify an output to the first framebuffer s.t. we can pass the colors to the fragment shader
layout(location = 0) out vec3 fragColor;

// Entry point for the shader
void main() {
    // For now, just quickly return the given vertex as the target vertex
    // The gl_Position is the global for the output position in frame space,
    //   and gl_VertexIndex specifies which vertex we're currently working on.
    //   Note that this means that the program won't work for more than three
    //   shaders!
    gl_Position = vec4(vertex_position, 0.0, 1.0);
    
    // Also pass the colour on
    fragColor = vertex_color;
}

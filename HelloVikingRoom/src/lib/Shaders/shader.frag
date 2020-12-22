/* FRAGMENT SHADER
 *   by Lut99
 *
 * Shader that is used to give a colour to the surviving fragments after
 * rasterization (i.e., making it pixels :) )
 */

#version 450
#extension GL_ARB_separate_shader_objects : enable

// Specify the input global to receive the colors from the vertex shader
layout(location = 0) in vec3 fragColor;
// Specify the output global. In our case, we only have one framebuffer, so we always take the zero'th index
layout(location = 0) out vec4 outColor;

// Entry point for the shader
void main() {
    // Simply set it to the color (as RGB vec3) we got from the vertex shader, with an alpha of 1.0
    outColor = vec4(fragColor, 1.0);
}

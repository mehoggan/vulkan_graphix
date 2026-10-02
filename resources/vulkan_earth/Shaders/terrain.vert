#version 450
// Port of VertexShader.vs.
#include "common.glsl"
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec2 out_texcoord;
void main() {
    out_normal = normalize(in_normal);
    out_texcoord = in_texcoord;
    gl_Position = draw_constants.mvp * vec4(in_position, 1.0);
}

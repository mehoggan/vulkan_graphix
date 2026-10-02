#version 450
// Port of VertexWater.vs; params.x is the wave timer.
#include "common.glsl"
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 0) out vec2 out_texcoord;
void main() {
    float timer = draw_constants.params.x;
    vec4 position = vec4(in_position, 1.0);
    position.y += sin(position.x / 5000.0 + timer) * 300.0;
    position.y += sin(position.z / 3000.0 + timer) * 300.0;
    position.y += sin(position.x / 632.0 + timer) * 30.0;
    position.y += sin(position.z / 341.0 + timer) * 30.0;
    out_texcoord = in_texcoord;
    gl_Position = draw_constants.mvp * position;
}

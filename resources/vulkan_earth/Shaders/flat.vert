#version 450
// Single-color meshes (glutSolidSphere shields/particles/explosions,
// VertexExplosion.vs): params is the RGBA color.
#include "common.glsl"
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
void main() {
    gl_Position = draw_constants.mvp * vec4(in_position, 1.0);
}

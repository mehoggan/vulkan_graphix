#version 450
// World-space colored/textured UI and debug geometry - the replacement for
// the game's immediate-mode glBegin()/glVertex3f()/glColor4f() quads,
// lines, and triangles.
#include "common.glsl"
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_texcoord;
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec2 out_texcoord;
void main() {
    out_color = in_color;
    out_texcoord = in_texcoord;
    gl_Position = draw_constants.mvp * vec4(in_position, 1.0);
}

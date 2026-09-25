#version 450

layout(set=0, binding=1) uniform sampler2D u_Atlas;

layout(location = 0) in vec2 v_Texcoord;
layout(location = 1) in vec4 v_Color;

layout(location = 0) out vec4 o_Color;

// Same trick as Tutorial15's shader.15.frag: glyph texels are white with
// coverage in alpha, and BitmapFont::solidTexelUv() is a reserved
// fully-opaque white texel, so one pipeline draws both glyph text and
// flat bevel-panel quads, tinted by the vertex color either way.
void main() {
    o_Color = texture(u_Atlas, v_Texcoord) * v_Color;
}

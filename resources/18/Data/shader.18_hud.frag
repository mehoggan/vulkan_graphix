#version 450

layout(set=0, binding=1) uniform sampler2D u_Atlas;

layout(location = 0) in vec2 v_Texcoord;
layout(location = 1) in vec4 v_Color;

layout(location = 0) out vec4 o_Color;

// Same solid-texel trick as Tutorial15/17: BitmapFont::solidTexelUv()
// lets flat-color bevel/bar quads go through the same textured pipeline
// as glyph text, tinted by vertex color.
void main() {
    o_Color = texture(u_Atlas, v_Texcoord) * v_Color;
}

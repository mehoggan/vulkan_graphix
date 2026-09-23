#version 450

layout(set=0, binding=1) uniform sampler2D u_Atlas;

layout(location = 0) in vec2 v_Texcoord;
layout(location = 1) in vec4 v_Color;

layout(location = 0) out vec4 o_Color;

// The single trick that makes one pipeline draw both glyph text and flat
// button-bevel quads: glyph texels are white with coverage in alpha, and
// BitmapFont::solidTexelUv() is a reserved fully-opaque white texel - so
// sampling either one and multiplying by v_Color just tints it, whether
// that's a glyph's antialiased edge or a flat bevel face.
void main() {
    o_Color = texture(u_Atlas, v_Texcoord) * v_Color;
}

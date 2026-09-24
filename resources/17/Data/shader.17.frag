#version 450

layout(set=0, binding=1) uniform sampler2D u_Atlas;

layout(location = 0) in vec2 v_Texcoord;
layout(location = 1) in vec4 v_Color;

layout(location = 0) out vec4 o_Color;

// Same one-pipeline-two-textures trick as Tutorial15's shader, extended to
// two separate atlases (font glyphs, item icons) bound as two separate
// descriptor sets in turn rather than as one combined atlas - see
// Tutorial17::prepareFrame()'s two bind+draw passes. Flat-color panel/
// bevel quads go through BitmapFont::solidTexelUv() exactly as in
// Tutorial15; icon quads sample real icon pixels with v_Color left at
// (1,1,1,1) so the icon's own colors show through unmodified.
void main() {
    o_Color = texture(u_Atlas, v_Texcoord) * v_Color;
}

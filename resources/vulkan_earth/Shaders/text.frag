#version 450
// BitmapFont text: the glyph atlas is white with coverage in alpha, so the
// vertex color is drawn at that coverage (alpha-blended).
layout(set = 0, binding = 0) uniform sampler2D glyph_atlas;
layout(location = 0) in vec4 in_color;
layout(location = 1) in vec2 in_texcoord;
layout(location = 0) out vec4 out_color;
void main() {
    float coverage = texture(glyph_atlas, in_texcoord).a;
    if (coverage <= 0.0) {
        discard;
    }
    out_color = vec4(in_color.rgb, in_color.a * coverage);
}

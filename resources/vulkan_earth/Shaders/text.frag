#version 450
// glBitmap semantics: a set glyph bit writes the raster color (opaque),
// an unset one writes nothing at all - not even depth.
layout(set = 0, binding = 0) uniform sampler2D glyph_atlas;
layout(location = 0) in vec4 in_color;
layout(location = 1) in vec2 in_texcoord;
layout(location = 0) out vec4 out_color;
void main() {
    if (texture(glyph_atlas, in_texcoord).r < 0.5) {
        discard;
    }
    out_color = vec4(in_color.rgb, 1.0);
}

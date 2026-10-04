#version 450
// Bitmap text: glyph quads already placed in normalized device
// coordinates (window pixels around the projected raster position) with
// the raster position's own depth, so mvp is not applied.
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_texcoord;
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec2 out_texcoord;
void main() {
    out_color = in_color;
    out_texcoord = in_texcoord;
    gl_Position = vec4(in_position, 1.0);
}

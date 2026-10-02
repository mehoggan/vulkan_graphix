#version 450
// GL_MODULATE texture environment (texel * vertex color); untextured
// geometry samples a 1x1 white texture. params.x = 1 selects GL_REPLACE
// (texel only) instead.
#include "common.glsl"
layout(set = 0, binding = 0) uniform sampler2D color_texture;
layout(location = 0) in vec4 in_color;
layout(location = 1) in vec2 in_texcoord;
layout(location = 0) out vec4 out_color;
void main() {
    vec4 texel = texture(color_texture, in_texcoord);
    out_color = draw_constants.params.x > 0.5 ? texel : texel * in_color;
}

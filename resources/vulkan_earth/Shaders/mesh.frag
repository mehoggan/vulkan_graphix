#version 450
// Port of FragmentTank.vs, which computes lighting but outputs only the
// texture color (its diffuse term is commented out) - so tanks are unlit.
// params.rgb tints the texel (1,1,1 = untinted); params.a is the alpha.
#include "common.glsl"
layout(set = 0, binding = 0) uniform sampler2D color_texture;
layout(location = 0) in vec2 in_texcoord;
layout(location = 0) out vec4 out_color;
void main() {
    vec3 texel = texture(color_texture, in_texcoord).rgb;
    out_color = vec4(texel * draw_constants.params.rgb, draw_constants.params.a);
}

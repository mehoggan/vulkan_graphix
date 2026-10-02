#version 450
// Port of FragmentExplosion.vs (gl_FragColor = gl_Color).
#include "common.glsl"
layout(location = 0) out vec4 out_color;
void main() {
    out_color = draw_constants.params;
}

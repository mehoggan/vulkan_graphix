#version 450
// Port of FragmentWater.vs: the texture color at alpha 0.7 (its lighting
// terms are computed but commented out of the result).
layout(set = 0, binding = 0) uniform sampler2D color_texture;
layout(location = 0) in vec2 in_texcoord;
layout(location = 0) out vec4 out_color;
void main() {
    out_color = vec4(texture(color_texture, in_texcoord).rgb, 0.7);
}

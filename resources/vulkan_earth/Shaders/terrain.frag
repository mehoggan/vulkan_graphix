#version 450
// Port of FragmentShader.vs: diffuse = max(ambient, N . -L) with the
// object-space normal, ambient = gl_LightSource[0].ambient.r = 0.3 and L =
// GL_LIGHT0's position (1, -1, 0), set once at startup under an identity
// modelview (VulkanEarth.cpp's initStuff()). The original also sampled a
// bump map it never used; that sample is dropped.
#include "common.glsl"
layout(set = 0, binding = 0) uniform sampler2D color_texture;
layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec2 in_texcoord;
layout(location = 0) out vec4 out_color;
void main() {
    const float ambient = 0.3;
    const vec3 light_direction = vec3(1.0, -1.0, 0.0);
    float diffuse = max(ambient, dot(in_normal, normalize(-light_direction)));
    out_color = vec4(diffuse * texture(color_texture, in_texcoord).rgb, 1.0);
}

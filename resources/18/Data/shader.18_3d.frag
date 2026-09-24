#version 450

layout(push_constant) uniform PushConstants {
    mat4 u_Model;
    vec4 u_Color;
};

layout(set=0, binding=0) uniform sampler2D u_Texture;

layout(location = 0) in vec2 v_Texcoord;

layout(location = 0) out vec4 o_Color;

void main() {
    o_Color = texture(u_Texture, v_Texcoord) * u_Color;
}

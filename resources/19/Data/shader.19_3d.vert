#version 450

// The active projectile's model matrix, set via vkCmdPushConstants
// immediately before each of this tutorial's three vkCmdDraw calls -
// same shape as Tutorial16's own push constant (no color tint needed
// here, unlike Tutorial18's tank tint).
layout(push_constant) uniform PushConstants {
    mat4 u_Model;
};

layout(set=0, binding=1) uniform u_UniformBuffer {
    mat4 u_View;
    mat4 u_Projection;
};

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec2 i_Texcoord;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec2 v_Texcoord;

void main() {
    gl_Position = u_Projection * u_View * u_Model * i_Position;
    v_Texcoord = i_Texcoord;
}

#version 450

// The active sphere instance's model matrix and flat color, set via
// vkCmdPushConstants immediately before each draw call - same shape as
// Tutorial18's push constant, reused here for particles/explosion/shield
// instead of tank parts.
layout(push_constant) uniform PushConstants {
    mat4 u_Model;
    vec4 u_Color;
};

layout(set=0, binding=0) uniform u_UniformBuffer {
    mat4 u_View;
    mat4 u_Projection;
};

layout(location = 0) in vec4 i_Position;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() {
    gl_Position = u_Projection * u_View * u_Model * i_Position;
}

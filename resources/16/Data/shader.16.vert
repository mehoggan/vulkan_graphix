#version 450

// Per-part model matrix, set via vkCmdPushConstants immediately before
// each of Tutorial16's three vkCmdDraw calls (body/head/turret) - the
// view+projection below are shared and only change with the orbit
// camera/window size, so they stay in the uniform buffer instead.
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

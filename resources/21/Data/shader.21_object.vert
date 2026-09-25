#version 450

// The active object's (tank part, or the skybox cube) model matrix, set
// via vkCmdPushConstants immediately before each draw call - matches
// Tutorial16's own push constant shape (no color tint needed).
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

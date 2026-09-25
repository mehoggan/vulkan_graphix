#version 450

// Per-part model matrix (already including the preview's own continuous
// Y-axis spin, see Tutorial22::getTankPartModelMatrix()), set via
// vkCmdPushConstants immediately before each of the three part draws -
// textually identical shape to Tutorial16's own shader.16.vert.
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

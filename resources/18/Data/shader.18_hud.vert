#version 450

layout(set=0, binding=0) uniform u_UniformBuffer {
    mat4 u_Projection;
};

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec2 i_Texcoord;
layout(location = 2) in vec4 i_Color;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec2 v_Texcoord;
layout(location = 1) out vec4 v_Color;

void main() {
    gl_Position = u_Projection * i_Position;
    v_Texcoord = i_Texcoord;
    v_Color = i_Color;
}

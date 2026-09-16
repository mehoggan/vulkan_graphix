#version 450

layout(set=0, binding=0) uniform u_UniformBuffer {
    mat4 u_View;
    mat4 u_Projection;
};

layout(location = 0) in vec4 i_Position;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() { gl_Position = u_Projection * u_View * i_Position; }

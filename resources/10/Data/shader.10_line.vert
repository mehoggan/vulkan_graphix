#version 450

layout(set=0, binding=0) uniform u_UniformBuffer {
    mat4 u_View;
    mat4 u_Projection;
    vec4 u_LightPosition;
    vec4 u_LightColor;
    vec4 u_ViewPosition;
};

layout(location = 0) in vec4 i_Position;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() {
    // Shares the tube's uniform buffer/descriptor set (view + projection)
    // even though the light fields go unused here - simpler than a second
    // descriptor set layout for a flat, unlit line.
    gl_Position = u_Projection * u_View * i_Position;
}

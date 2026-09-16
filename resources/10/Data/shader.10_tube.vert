#version 450

layout(set=0, binding=0) uniform u_UniformBuffer {
    mat4 u_View;
    mat4 u_Projection;
    vec4 u_LightPosition;
    vec4 u_LightColor;
    vec4 u_ViewPosition;
};

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec3 i_Normal;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec3 v_WorldPosition;
layout(location = 1) out vec3 v_Normal;

void main() {
    // The tube's ring vertices are generated directly in world space (see
    // Tutorial10::getTubeVertexData()), so there is no model matrix here.
    gl_Position = u_Projection * u_View * i_Position;
    v_WorldPosition = i_Position.xyz;
    v_Normal = normalize(i_Normal);
}

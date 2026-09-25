#version 450

layout(set=0, binding=0) uniform u_UniformBuffer {
    mat4 u_Model;
    mat4 u_View;
    mat4 u_Projection;
    vec4 u_LightPosition;
    vec4 u_LightColor;
    vec4 u_ViewPosition;
};

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec3 i_Normal;
layout(location = 2) in vec2 i_Texcoord;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec3 v_WorldPosition;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_Texcoord;

void main() {
    vec4 world_position = u_Model * i_Position;
    gl_Position = u_Projection * u_View * world_position;
    v_WorldPosition = world_position.xyz;

    mat3 normal_matrix = mat3(transpose(inverse(u_Model)));
    v_Normal = normalize(normal_matrix * i_Normal);
    v_Texcoord = i_Texcoord;
}

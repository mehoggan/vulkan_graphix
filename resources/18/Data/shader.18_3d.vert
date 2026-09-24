#version 450

// The active tank instance's model matrix and team-color tint, set via
// vkCmdPushConstants immediately before each of this tutorial's six
// vkCmdDraw calls (2 tanks x 3 parts) - extends Tutorial16's own
// Mat4-only push constant with a Vec4 tint (Player::color[4] is real,
// though the specific red/blue values here are this tutorial's own
// illustrative team-color choice - see Tutorial18.h).
layout(push_constant) uniform PushConstants {
    mat4 u_Model;
    vec4 u_Color;
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

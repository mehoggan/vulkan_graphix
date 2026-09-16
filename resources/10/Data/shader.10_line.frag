#version 450

// First tutorial with push constants: a single flat color for the control
// polygon, set once per draw call via vkCmdPushConstants rather than
// needing a second uniform buffer/descriptor set.
layout(push_constant) uniform PushConstants {
    vec4 u_Color;
}
u_PushConstants;

layout(location = 0) out vec4 o_Color;

void main() { o_Color = u_PushConstants.u_Color; }

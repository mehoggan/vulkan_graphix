#version 450

layout(push_constant) uniform PushConstants {
    mat4 u_Model;
    vec4 u_Color;
};

layout(location = 0) out vec4 o_Color;

// Unlit, flat-colored - matches Particle::draw()/Explosion::draw()'s own
// glColor4f(...); glutSolidSphere(...) (GL_LIGHTING is only ever enabled
// for the terrain in the real game, never for particles/explosions/
// status-effect spheres - see Tutorial21's own header comment).
void main() {
    o_Color = u_Color;
}

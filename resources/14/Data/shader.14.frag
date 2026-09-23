#version 450

layout(set=0, binding=0) uniform u_UniformBuffer {
    mat4 u_Model;
    mat4 u_View;
    mat4 u_Projection;
    vec4 u_LightPosition;
    vec4 u_LightColor;
    vec4 u_ViewPosition;
};

layout(location = 0) in vec3 v_WorldPosition;
layout(location = 1) in vec3 v_Normal;

layout(location = 0) out vec4 o_Color;

// Matches Particle::render()'s own glColor4f(r, g, b, 0.4) - a flat,
// translucent particle color (here a representative orange, in the same
// family as ParticleSmoke/ParticleAcid's own explosion-effect palette),
// alpha-blended over whatever is behind it instead of textured. The Phong
// terms below are modeled on Tutorial08's own lighting math (see
// resources/08/Data/shader.08.frag), just applied to this flat color
// instead of Tutorial08's hardcoded opaque one.
const vec3 c_ParticleColor = vec3(0.9, 0.45, 0.15);
const float c_ParticleAlpha = 0.4;
const float c_AmbientStrength = 0.35;
const float c_SpecularStrength = 0.4;
const float c_Shininess = 16.0;

void main() {
    vec3 normal = normalize(v_Normal);
    vec3 light_dir = normalize(u_LightPosition.xyz - v_WorldPosition);
    vec3 view_dir = normalize(u_ViewPosition.xyz - v_WorldPosition);
    vec3 reflect_dir = reflect(-light_dir, normal);

    vec3 ambient = c_AmbientStrength * u_LightColor.rgb;

    float diffuse_factor = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diffuse_factor * u_LightColor.rgb;

    float specular_factor =
            pow(max(dot(view_dir, reflect_dir), 0.0), c_Shininess);
    vec3 specular = c_SpecularStrength * specular_factor * u_LightColor.rgb;

    vec3 result = (ambient + diffuse + specular) * c_ParticleColor;
    o_Color = vec4(result, c_ParticleAlpha);
}

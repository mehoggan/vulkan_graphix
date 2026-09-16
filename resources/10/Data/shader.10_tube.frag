#version 450

layout(set=0, binding=0) uniform u_UniformBuffer {
    mat4 u_View;
    mat4 u_Projection;
    vec4 u_LightPosition;
    vec4 u_LightColor;
    vec4 u_ViewPosition;
};

layout(location = 0) in vec3 v_WorldPosition;
layout(location = 1) in vec3 v_Normal;

layout(location = 0) out vec4 o_Color;

// Vibrant teal/cyan - distinct from Tutorial08's warm red-orange sphere and
// Tutorial09's earthy terrain palette.
const vec3 c_ObjectColor = vec3(0.1, 0.85, 0.8);
const float c_AmbientStrength = 0.2;
const float c_SpecularStrength = 0.5;
const float c_Shininess = 32.0;

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

    vec3 result = (ambient + diffuse + specular) * c_ObjectColor;
    o_Color = vec4(result, 1.0);
}

#version 450

layout(set=0, binding=0) uniform u_UniformBuffer {
    mat4 u_Model;
    mat4 u_View;
    mat4 u_Projection;
    vec4 u_LightPosition;
    vec4 u_LightColor;
    vec4 u_ViewPosition;
};

layout(set=0, binding=1) uniform sampler2D u_Texture;

layout(location = 0) in vec3 v_WorldPosition;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_Texcoord;

layout(location = 0) out vec4 o_Color;

const float c_AmbientStrength = 0.2;
const float c_SpecularStrength = 0.3;
const float c_Shininess = 16.0;

void main() {
    // The mesh has backface culling disabled, so fragments can be shaded
    // from either side of a triangle; flip the normal to face the camera
    // so the underside isn't lit as if it were pointing away from it.
    vec3 normal = normalize(v_Normal);
    vec3 view_dir = normalize(u_ViewPosition.xyz - v_WorldPosition);
    if (dot(normal, view_dir) < 0.0) {
        normal = -normal;
    }

    vec3 light_dir = normalize(u_LightPosition.xyz - v_WorldPosition);
    vec3 reflect_dir = reflect(-light_dir, normal);

    vec3 object_color = texture(u_Texture, v_Texcoord).rgb;

    vec3 ambient = c_AmbientStrength * u_LightColor.rgb;

    float diffuse_factor = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diffuse_factor * u_LightColor.rgb;

    float specular_factor =
            pow(max(dot(view_dir, reflect_dir), 0.0), c_Shininess);
    vec3 specular = c_SpecularStrength * specular_factor * u_LightColor.rgb;

    vec3 result = (ambient + diffuse + specular) * object_color;
    o_Color = vec4(result, 1.0);
}

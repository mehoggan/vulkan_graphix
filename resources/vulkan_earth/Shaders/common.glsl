// Shared by every vulkan_earth shader: one push-constant block per draw
// (the full model-view-projection, precomputed on the CPU, plus four
// draw-specific parameters) and, where a shader samples, one combined
// image sampler at set 0 / binding 0.
layout(push_constant) uniform DrawConstants {
    mat4 mvp;
    vec4 params;
} draw_constants;

#ifndef VULKAN_EARTH_RENDER_RENDER_TYPES_H
#define VULKAN_EARTH_RENDER_RENDER_TYPES_H

// Types shared across vulkan_earth's Vulkan renderer (see Renderer.h for
// the overall design).

#include <cstdint>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_earth::render {

using Vec2 = vulkan_graphix::Math::Vec2<float>;
using Vec3 = vulkan_graphix::Math::Vec3<float>;
using Vec4 = vulkan_graphix::Math::Vec4<float>;
using Mat4 = vulkan_graphix::Math::Mat4<float>;

// UI, text, and debug geometry: what the game used to emit through
// glVertex3f()/glColor4f()/glTexCoord2f().
struct UiVertex {
    Vec3 position;
    Vec4 color;
    Vec2 texcoord;
};

// Lit/textured meshes (.ogl models, terrain, water, skybox, spheres).
struct MeshVertex {
    Vec3 position;
    Vec3 normal;
    Vec2 texcoord;
};

// Every pipeline's push-constant block (shaders' common.glsl): the full
// model-view-projection, precomputed on the CPU, plus four draw-specific
// parameters (a tint, a color, a timer - see each shader).
struct DrawConstants {
    Mat4 mvp;
    Vec4 params;
};

enum class PipelineId : std::uint32_t {
    UiTriangles,
    UiLines,
    Text,
    Mesh,
    // Mesh, drawn without depth testing or writing (the skybox).
    MeshBackground,
    Terrain,
    Water,
    FlatColor,
    // FlatColor rasterized as lines (the terrain's wireframe view; falls
    // back to FlatColor where fillModeNonSolid is unsupported).
    FlatColorWireframe,
    Count
};

}  // namespace vulkan_earth::render

#endif  // VULKAN_EARTH_RENDER_RENDER_TYPES_H

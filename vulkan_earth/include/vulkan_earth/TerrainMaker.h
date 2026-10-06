#ifndef VULKAN_EARTH_TERRAINMAKER_H
#define VULKAN_EARTH_TERRAINMAKER_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"
#include "vulkan_graphix/Render/Texture.h"
#include "vulkan_graphix/TerrainGenerator.h"

// The height field itself - generation, normals, world-position queries,
// and crater deformation - is libvulkan_graphix's TerrainGenerator (shared
// with the Vulkan tutorials); TerrainMaker keeps the mesh built from it (a
// device-local vertex buffer, rebuilt after generation or a crater), the
// color texture, and thin wrappers converting to this game's own
// vulkan_graphix::Math::Vec3<float> type.

namespace vulkan_graphix::Render {
class RenderContext;
}

class TerrainMaker {
public:
    TerrainMaker(std::int32_t i_scale, std::int32_t i_size);
    ~TerrainMaker();
    void draw(vulkan_graphix::Render::RenderContext& context);
    void initData();
    void prepareData(std::int32_t new_steps,
      std::int32_t new_increase,
      float new_radius,
      std::int32_t new_random_jump,
      std::int32_t smoothness);
    void stdMessageBox(const std::string& output);
    void errorMessageBox(const std::string& output);
    void toggleWireframe();
    void makeCrater(float x, float z, float size);
    // x/z in grid units (truncated to a grid vertex).
    vulkan_graphix::Math::Vec3<float> getTriangleNormal(float x, float z);
    // x/z in world units.
    vulkan_graphix::Math::Vec3<float> getNormalAt(float x, float z);
    float getHeightAt(float x, float z);
    std::int32_t getActualSize();
    std::int32_t getScale();
    // Switches the terrain's color texture: "Rock", "Snow", "Ice", "Mars",
    // "Desert", or "Lava".
    void selectTexture(const std::string& tex);

private:
    void rebuildMesh();

    std::int32_t m_scale;
    std::int32_t m_size;
    std::int32_t m_steps;
    std::int32_t m_increase;
    float m_radius;
    std::int32_t m_random_jump;
    std::int32_t m_total_vertices;
    std::int32_t m_tri_strip_buffer_size;
    vulkan_graphix::TerrainGenerator m_terrain;
    std::vector<vulkan_graphix::Math::Vec3<float>> m_vertices;
    std::vector<vulkan_graphix::Math::Vec3<float>> m_normals;
    std::vector<vulkan_graphix::Math::Vec2<float>> m_tex_coord;
    std::shared_ptr<vulkan_graphix::Render::Texture> m_color_texture;
    std::unique_ptr<vulkan_graphix::Render::Mesh> m_mesh;
    // The wireframe view's per-vertex debug normals.
    vulkan_graphix::Render::UiMesh m_normal_lines;
    bool m_mesh_dirty = true;
    float m_rotation_angle;
    bool m_wireframe_active;
};

#endif  // VULKAN_EARTH_TERRAINMAKER_H
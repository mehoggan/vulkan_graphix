#ifndef TERRAINMAKER
#define TERRAINMAKER

#include <stdio.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "vulkan_earth/Normal.h"
#include "vulkan_earth/TexCoord.h"
#include "vulkan_earth/Vertex.h"
#include "vulkan_earth/render/Mesh.h"
#include "vulkan_earth/render/Texture.h"
#include "vulkan_graphix/TerrainGenerator.h"

// The height field itself - generation, normals, world-position queries,
// and crater deformation - is libvulkan_graphix's TerrainGenerator (shared
// with the Vulkan tutorials); TerrainMaker keeps the mesh built from it (a
// device-local vertex buffer, rebuilt after generation or a crater), the
// color texture, and thin wrappers converting to this game's own Normal
// type.

namespace vulkan_earth::render {
class RenderContext;
}

class TerrainMaker {
public:
    TerrainMaker(std::int32_t i_scale, std::int32_t i_size);
    ~TerrainMaker();
    void draw(vulkan_earth::render::RenderContext& context);
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
    Normal getTriangleNormal(float x, float z);
    // x/z in world units.
    Normal getNormalAt(float x, float z);
    float getHeightAt(float x, float z);
    std::int32_t getActualSize();
    std::int32_t getScale();
    // Switches the terrain's color texture: "Rock", "Snow", "Ice", "Mars",
    // "Desert", or "Lava".
    void selectTexture(const std::string& tex);

private:
    void rebuildMesh();

    std::int32_t scale;
    std::int32_t size;
    std::int32_t steps;
    std::int32_t increase;
    float radius;
    std::int32_t random_jump;
    std::int32_t total_vertices;
    std::int32_t tri_strip_buffer_size;
    vulkan_graphix::TerrainGenerator terrain;
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<TexCoord> tex_coord;
    std::shared_ptr<vulkan_earth::render::Texture> color_texture;
    std::unique_ptr<vulkan_earth::render::StaticMesh> mesh;
    // The wireframe view's per-vertex debug normals.
    vulkan_earth::render::UiMesh normal_lines;
    bool mesh_dirty = true;
    float rotation_angle;
    bool wireframe_active;
};

#endif  //	TERRAINMAKER
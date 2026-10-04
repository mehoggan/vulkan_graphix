#ifndef VULKAN_EARTH_WATER_H
#define VULKAN_EARTH_WATER_H

#include <cstdint>
#include <memory>
#include <vector>
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"
#include "vulkan_graphix/Render/Texture.h"

namespace vulkan_graphix::Render {
class RenderContext;
}

class Water {
public:
    Water();
    Water(std::int32_t new_scale, std::int32_t new_size);
    ~Water();
    void draw(vulkan_graphix::Render::RenderContext& context);
    void initData();
    void prepareData(std::int32_t steps,
                     std::int32_t increase,
                     float radius,
                     std::int32_t random_jump);
    void calcAverageofSixNormals(vulkan_graphix::Math::Vec3<float>* v_0,
                                 float x1,
                                 float y1,
                                 float z1,
                                 float x2,
                                 float y2,
                                 float z2,
                                 float x3,
                                 float y3,
                                 float z3,
                                 float x4,
                                 float y4,
                                 float z4,
                                 float x5,
                                 float y5,
                                 float z5,
                                 float x6,
                                 float y6,
                                 float z6,
                                 vulkan_graphix::Math::Vec3<float>* n);
    void prepTerrain();
    void prepareData();
    void terrainGen(std::int32_t steps,
                    std::int32_t increase,
                    float radius,
                    std::int32_t random_jump);
    std::int32_t getActualSize();
    std::int32_t getScale();
    void stdMessageBox(const std::string& output);
    void errorMessageBox(const std::string& output);

private:
    std::int32_t m_scale;
    std::int32_t m_size;
    std::int32_t m_total_vertices;
    std::int32_t m_tri_strip_buffer_size;
    std::int32_t** m_surfaceheight;
    std::vector<vulkan_graphix::Math::Vec3<float>> m_vertices;
    std::vector<vulkan_graphix::Math::Vec3<float>> m_normals;
    std::vector<vulkan_graphix::Math::Vec2<float>> m_tex_coord;
    std::shared_ptr<vulkan_graphix::Render::Texture> m_color_texture;
    std::unique_ptr<vulkan_graphix::Render::Mesh> m_mesh;
    float m_timer;
};

#endif  // VULKAN_EARTH_WATER_H

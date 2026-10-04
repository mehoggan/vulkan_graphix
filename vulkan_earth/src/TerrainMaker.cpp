#include "vulkan_earth/TerrainMaker.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include "math.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

TerrainMaker::TerrainMaker(std::int32_t i_scale, std::int32_t i_size) :
        m_terrain(i_size, i_scale) {
    srand(time(nullptr));
    m_scale = i_scale;
    m_size = i_size;
    m_total_vertices = m_size * m_size;
    m_tri_strip_buffer_size = (m_size - 1) * (m_size - 1) * 6;
    initData();

    // GL_LINEAR filtering, GL_REPEAT wrapping. (The original also loaded
    // bumpMap.raw as a normal map, which its terrain shader sampled but
    // never used.)
    m_color_texture = render::Renderer::instance().loadRawTexture(
            "Rocky.raw", 2048, 2048, VK_SAMPLER_ADDRESS_MODE_REPEAT);

    m_rotation_angle = 0.0;
    m_wireframe_active = false;
}

TerrainMaker::~TerrainMaker() = default;

std::int32_t TerrainMaker::getScale() { return m_scale; }
std::int32_t TerrainMaker::getActualSize() { return (m_size) * (m_scale); }

// The original deleted the current texture and returned the newly loaded
// one, which its only caller discarded - it worked only because GL happened
// to reuse the freed texture name. This stores the new texture directly.
void TerrainMaker::selectTexture(const std::string& tex) {
    const char* filename = nullptr;
    if (tex == "Rock")
        filename = "Rocky.raw";
    else if (tex == "Snow")
        filename = "Snowy.raw";
    else if (tex == "Ice")
        filename = "Icy.raw";
    else if (tex == "Mars")
        filename = "RedPlanet.raw";
    else if (tex == "Desert")
        filename = "Desert.raw";
    else if (tex == "Lava")
        filename = "LavaRock.raw";
    if (filename != nullptr) {
        m_color_texture = render::Renderer::instance().loadRawTexture(
                filename, 2048, 2048, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    }
}

void TerrainMaker::rebuildMesh() {
    std::vector<render::MeshVertex> mesh_vertices(m_vertices.size());
    for (std::size_t i = 0; i < m_vertices.size(); ++i) {
        mesh_vertices[i] = {
                m_vertices[i],
                m_normals[i],
                math::Vec2<float>(m_tex_coord[i].s, m_tex_coord[i].t)};
    }
    m_mesh = render::Renderer::instance().createMesh(mesh_vertices);

    // The wireframe view's debug normals: one 500-unit line per fourth grid
    // vertex.
    const std::int32_t draw_scale = m_scale;
    m_normal_lines.clear();
    const math::Vec4<float> white(1, 1, 1, .75);
    for (std::int32_t i = 0; i < 255; i += 4) {
        for (std::int32_t j = 0; j < 255; j += 4) {
            const vulkan_graphix::Math::Vec3<float> normal =
                    getNormalAt(i * draw_scale, j * draw_scale);
            m_normal_lines.addLine(
                    math::Vec3<float>(i * draw_scale,
                                      m_terrain.heightAt(i, j),
                                      j * draw_scale),
                    math::Vec3<float>(
                            i * draw_scale + 500 * normal.x,
                            m_terrain.heightAt(i, j) + 500 * normal.y,
                            j * draw_scale + 500 * normal.z),
                    white);
        }
    }
    m_mesh_dirty = false;
}

void TerrainMaker::draw(render::RenderContext& context) {
    if (m_mesh_dirty) {
        rebuildMesh();
    }
    if (!m_mesh) {
        return;
    }
    if (m_wireframe_active) {
        // Untextured, unlit, line-mode triangles in translucent black, then
        // the debug normals.
        context.drawMesh(*m_mesh,
                         vulkan_earth::pipelines().m_flat_color_wireframe,
                         nullptr,
                         math::Mat4<float>(1.0f),
                         math::Vec4<float>(0, 0, 0, .75));
        context.draw(m_normal_lines);
    } else {
        context.drawMesh(*m_mesh,
                         vulkan_earth::pipelines().m_terrain,
                         m_color_texture.get(),
                         math::Mat4<float>(1.0f),
                         math::Vec4<float>(1.0f));
    }
}

void TerrainMaker::initData() {
    m_vertices.resize(m_tri_strip_buffer_size);
    m_normals.resize(m_tri_strip_buffer_size);
    m_tex_coord.resize(m_tri_strip_buffer_size);
}

void TerrainMaker::prepareData(std::int32_t new_steps,
                               std::int32_t new_increase,
                               float new_radius,
                               std::int32_t new_random_jump,
                               std::int32_t smoothness) {
    std::int32_t chunk_size = m_size / 2;
    m_steps = new_steps;
    m_increase = new_increase;
    m_radius = new_radius;
    m_random_jump = new_random_jump;
    std::int32_t buffersize = m_tri_strip_buffer_size;
    std::int32_t prep_size = m_size;
    std::int32_t prep_scale = m_scale;
    m_terrain.generate(
            new_steps, new_increase, new_radius, new_random_jump, smoothness);

    //
    // 				v_k
    //	v_i			v_z
    //		x-----x
    //		|    /|
    //		|  /  |
    //		|/	  |
    //		x-----x
    //	v_j			v_y
    //	v_x
    std::int32_t index = 0;
    std::int32_t index_normals = 0;
    std::int32_t index_texture = 0;
    for (std::int32_t i = 0; i < prep_size - 1; i++) {
        for (std::int32_t j = 0; j < prep_size - 1; j++) {
            /************************************************************/
            /*	V_I -- N_I												*/
            /************************************************************/
            vulkan_graphix::Math::Vec3<float> v_i(
                    j * prep_scale,
                    m_terrain.heightAt(j, i) /*SCALE*/,
                    i * prep_scale);
            m_vertices[index++] = v_i;
            vulkan_graphix::Math::Vec2<float> t_i(
                    (static_cast<float>(i % (chunk_size - 1))) /
                            static_cast<float>(chunk_size - 1),
                    (static_cast<float>(j % (chunk_size - 1))) /
                            static_cast<float>(chunk_size - 1));
            m_tex_coord[index_texture++] = t_i;
            vulkan_graphix::Math::Vec3<float> n_i(0, 0, 0);
            if (i == 0 && j == 0) {
            } else if (i == 0) {
            } else if (j == 0) {
            } else {
                n_i = m_terrain.normalAt(j, i);
            }
            m_normals[index_normals++] = n_i;

            /************************************************************/
            /*	V_J -- N_J												*/
            /************************************************************/
            vulkan_graphix::Math::Vec3<float> v_j(
                    j * prep_scale,
                    m_terrain.heightAt(j, i + 1) /*SCALE*/,
                    (i + 1) * prep_scale);
            m_vertices[index++] = v_j;
            vulkan_graphix::Math::Vec2<float> t_j(
                    (static_cast<float>(i % (chunk_size - 1)) + 1) /
                            static_cast<float>(chunk_size - 1),
                    (static_cast<float>(j % (chunk_size - 1))) /
                            static_cast<float>(chunk_size - 1));
            m_tex_coord[index_texture++] = t_j;
            vulkan_graphix::Math::Vec3<float> n_j(0, 0, 0);
            if (i == prep_size - 2 && j == 0) {
            } else if (j == 0) {
            } else if (i == prep_size - 2) {
            } else {
                n_j = m_terrain.normalAt(j, i + 1);
            }
            m_normals[index_normals++] = n_j;

            /************************************************************/
            /*	V_K -- N_K												*/
            /************************************************************/
            vulkan_graphix::Math::Vec3<float> v_k(
                    (j + 1) * prep_scale,
                    m_terrain.heightAt(j + 1, i) /*SCALE*/,
                    (i)*prep_scale);
            m_vertices[index++] = v_k;
            vulkan_graphix::Math::Vec2<float> t_k(
                    (static_cast<float>(i % (chunk_size - 1))) /
                            static_cast<float>(chunk_size - 1),
                    (static_cast<float>(j % (chunk_size - 1)) + 1) /
                            static_cast<float>(chunk_size - 1));
            m_tex_coord[index_texture++] = t_k;
            vulkan_graphix::Math::Vec3<float> n_k(0, 0, 0);
            if (i == 0 && j == prep_size - 2) {
            } else if (i == 0) {
            } else if (j == prep_size - 2) {
            } else {
                n_k = m_terrain.normalAt(j + 1, i);
            }
            m_normals[index_normals++] = n_k;

            /************************************************************/
            /*	V_X -- N_X	(SAME AS V_J/N_J)							*/
            /************************************************************/
            vulkan_graphix::Math::Vec3<float> v_x(
                    j * prep_scale,
                    m_terrain.heightAt(j, i + 1) /*SCALE*/,
                    (i + 1) * prep_scale);
            m_vertices[index++] = v_x;
            vulkan_graphix::Math::Vec2<float> t_x(
                    (static_cast<float>(i % (chunk_size - 1)) + 1) /
                            static_cast<float>(chunk_size - 1),
                    (static_cast<float>(j % (chunk_size - 1))) /
                            static_cast<float>(chunk_size - 1));
            m_tex_coord[index_texture++] = t_x;
            vulkan_graphix::Math::Vec3<float> n_x(0, 0, 0);
            if (i == prep_size - 2 && j == 0) {
            } else if (j == 0) {
            } else if (i == prep_size - 2) {
            } else {
                n_x = m_terrain.normalAt(j, i + 1);
            }
            m_normals[index_normals++] = n_x;

            /************************************************************/
            /*	V_Y -- N_Y												*/
            /************************************************************/
            vulkan_graphix::Math::Vec3<float> v_y(
                    (j + 1) * prep_scale,
                    m_terrain.heightAt(j + 1, i + 1) /*SCALE*/,
                    (i + 1) * prep_scale);
            m_vertices[index++] = v_y;
            vulkan_graphix::Math::Vec2<float> t_y(
                    (static_cast<float>(i % (chunk_size - 1)) + 1) /
                            static_cast<float>(chunk_size - 1),
                    (static_cast<float>(j % (chunk_size - 1)) + 1) /
                            static_cast<float>(chunk_size - 1));
            m_tex_coord[index_texture++] = t_y;
            vulkan_graphix::Math::Vec3<float> n_y(0, 0, 0);
            if (i == prep_size - 2 && j == prep_size - 2) {
            } else if (i == prep_size - 2) {
            } else if (j == prep_size - 2) {
            } else {
                n_y = m_terrain.normalAt(j + 1, i + 1);
            }
            m_normals[index_normals++] = n_y;

            /************************************************************/
            /*	V_Z -- N_Z												*/
            /************************************************************/
            vulkan_graphix::Math::Vec3<float> v_z(
                    (j + 1) * prep_scale,
                    m_terrain.heightAt(j + 1, i) /*SCALE*/,
                    (i)*prep_scale);
            m_vertices[index++] = v_z;
            vulkan_graphix::Math::Vec2<float> t_z(
                    (static_cast<float>(i % (chunk_size - 1))) /
                            static_cast<float>(chunk_size - 1),
                    (static_cast<float>(j % (chunk_size - 1)) + 1) /
                            static_cast<float>(chunk_size - 1));
            m_tex_coord[index_texture++] = t_z;
            vulkan_graphix::Math::Vec3<float> n_z(0, 0, 0);
            if (i == 0 && j == prep_size - 2) {
            } else if (i == 0) {
            } else if (j == prep_size - 2) {
            } else {
                n_z = m_terrain.normalAt(j + 1, i);
            }
            m_normals[index_normals++] = n_z;
        }
    }

    m_mesh_dirty = true;
}

void TerrainMaker::toggleWireframe() {
    m_wireframe_active = !m_wireframe_active;
}

vulkan_graphix::Math::Vec3<float> TerrainMaker::getTriangleNormal(float x,
                                                                  float z) {
    return m_terrain.triangleNormalAt(static_cast<std::int32_t>(x),
                                      static_cast<std::int32_t>(z));
}

vulkan_graphix::Math::Vec3<float> TerrainMaker::getNormalAt(float x, float z) {
    return m_terrain.normalAtWorld(x, z);
}

float TerrainMaker::getHeightAt(float x, float z) {
    return m_terrain.heightAtWorld(x, z);
}

// The crater's height math is TerrainGenerator::makeCrater(); this only
// rewrites the affected grid vertices in the mesh. prepareData() lays
// the mesh out as six triangle-list slots per grid cell (row z, column x),
// so each grid vertex is repeated in up to six slots across the cells
// around it - the slot choice (including skipping row/column 0's
// neighbors via `> 0`) is TerrainMaker's own. Unlike the original, a slot
// outside the (size - 1) x (size - 1) cell grid is skipped rather than
// written past the end of the buffer. The patched arrays are re-uploaded
// as a whole on the next draw().
void TerrainMaker::makeCrater(float impact_x,
                              float impact_z,
                              float blast_size) {
    const std::vector<vulkan_graphix::TerrainGridCell> cells =
            m_terrain.makeCrater(impact_x, impact_z, blast_size);

    const std::int32_t cells_per_row = m_size - 1;
    auto slot = [&](std::int32_t row,
                    std::int32_t col,
                    std::int32_t corner) -> std::int32_t {
        if (row < 0 || col < 0 || row >= cells_per_row ||
            col >= cells_per_row) {
            return -1;
        }
        return (row * cells_per_row + col) * 6 + corner;
    };
    // Every slot grid vertex (i, j) occupies, per TerrainMaker's layout.
    auto slots_for = [&](std::int32_t grid_x, std::int32_t grid_z) {
        std::vector<std::int32_t> slots = {slot(grid_z, grid_x, 0)};
        if ((grid_z - 1) > 0) {
            slots.push_back(slot(grid_z - 1, grid_x, 1));
            slots.push_back(slot(grid_z - 1, grid_x, 3));
            if ((grid_x - 1) > 0) {
                slots.push_back(slot(grid_z - 1, grid_x - 1, 4));
            }
        }
        if ((grid_x - 1) > 0) {
            slots.push_back(slot(grid_z, grid_x - 1, 2));
            slots.push_back(slot(grid_z, grid_x - 1, 5));
        }
        return slots;
    };

    for (const vulkan_graphix::TerrainGridCell& cell : cells) {
        const float height =
                static_cast<float>(m_terrain.heightAt(cell.m_x, cell.m_z));
        for (std::int32_t index : slots_for(cell.m_x, cell.m_z)) {
            if (index >= 0) {
                m_vertices[index].y = height;
            }
        }
    }

    for (const vulkan_graphix::TerrainGridCell& cell : cells) {
        const vulkan_graphix::Math::Vec3<float> normal =
                m_terrain.normalAt(cell.m_x, cell.m_z);
        for (std::int32_t index : slots_for(cell.m_x, cell.m_z)) {
            if (index >= 0) {
                m_normals[index] = normal;
            }
        }
    }
    m_mesh_dirty = true;
}

/************************************************************************/
/*	Debugging MessageBox functions										*/
/************************************************************************/
void TerrainMaker::stdMessageBox(const std::string& output) {}

void TerrainMaker::errorMessageBox(const std::string& output) {}

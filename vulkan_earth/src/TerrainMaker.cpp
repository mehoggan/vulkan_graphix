#include "vulkan_earth/TerrainMaker.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include "math.h"
#include "vulkan_earth/Normal.h"
#include "vulkan_earth/TexCoord.h"
#include "vulkan_earth/Vertex.h"
#include "vulkan_earth/render/Renderer.h"
#include "vulkan_earth/render/UiBuilders.h"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace render = vulkan_earth::render;

namespace {
Normal toNormal(vulkan_graphix::Math::Vec3<float> const& normal) {
    return Normal(normal.x, normal.y, normal.z);
}
}  // namespace

TerrainMaker::TerrainMaker(std::int32_t i_scale, std::int32_t i_size)
        : terrain(i_size, i_scale) {
    srand(time(nullptr));
    scale = i_scale;
    size = i_size;
    total_vertices = size * size;
    tri_strip_buffer_size = (size - 1) * (size - 1) * 6;
    initData();

    // GL_LINEAR filtering, GL_REPEAT wrapping. (The original also loaded
    // bumpMap.raw as a normal map, which its terrain shader sampled but
    // never used.)
    color_texture = render::Renderer::instance().loadRawTexture(
            "Rocky.raw", 2048, 2048, VK_SAMPLER_ADDRESS_MODE_REPEAT);

    rotation_angle = 0.0;
    wireframe_active = false;
}

TerrainMaker::~TerrainMaker() = default;

std::int32_t TerrainMaker::getScale() { return scale; }
std::int32_t TerrainMaker::getActualSize() { return (size) * (scale); }

// The original deleted the current texture and returned the newly loaded
// one, which its only caller discarded - it worked only because GL happened
// to reuse the freed texture name. This stores the new texture directly.
void TerrainMaker::selectTexture(const std::string& tex) {
    char const* filename = nullptr;
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
        color_texture = render::Renderer::instance().loadRawTexture(
                filename, 2048, 2048, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    }
}

void TerrainMaker::rebuildMesh() {
    std::vector<render::MeshVertex> mesh_vertices(vertices.size());
    for (std::size_t i = 0; i < vertices.size(); ++i) {
        mesh_vertices[i] = {render::Vec3(vertices[i].coord_x,
                                         vertices[i].coord_y,
                                         vertices[i].coord_z),
                            render::Vec3(normals[i].compo_x,
                                         normals[i].compo_y,
                                         normals[i].compo_z),
                            render::Vec2(tex_coord[i].texcoord_s,
                                         tex_coord[i].texcoord_t)};
    }
    mesh = render::Renderer::instance().createMesh(mesh_vertices);

    // The wireframe view's debug normals: one 500-unit line per fourth grid
    // vertex.
    std::int32_t const draw_scale = scale;
    normal_lines.clear();
    render::Vec4 const white(1, 1, 1, .75);
    for (std::int32_t i = 0; i < 255; i += 4) {
        for (std::int32_t j = 0; j < 255; j += 4) {
            Normal const normal = getNormalAt(i * draw_scale, j * draw_scale);
            normal_lines.addLine(
                    render::Vec3(i * draw_scale,
                                 terrain.heightAt(i, j),
                                 j * draw_scale),
                    render::Vec3(i * draw_scale + 500 * normal.compo_x,
                                 terrain.heightAt(i, j) + 500 * normal.compo_y,
                                 j * draw_scale + 500 * normal.compo_z),
                    white);
        }
    }
    mesh_dirty = false;
}

void TerrainMaker::draw(render::RenderContext& context) {
    if (mesh_dirty) {
        rebuildMesh();
    }
    if (!mesh) {
        return;
    }
    if (wireframe_active) {
        // Untextured, unlit, line-mode triangles in translucent black, then
        // the debug normals.
        context.drawMesh(*mesh,
                         render::PipelineId::FlatColorWireframe,
                         nullptr,
                         render::Mat4(1.0f),
                         render::Vec4(0, 0, 0, .75));
        context.draw(normal_lines);
    } else {
        context.drawMesh(*mesh,
                         render::PipelineId::Terrain,
                         color_texture.get(),
                         render::Mat4(1.0f),
                         render::Vec4(1.0f));
    }
}

void TerrainMaker::initData() {
    vertices.resize(tri_strip_buffer_size);
    normals.resize(tri_strip_buffer_size);
    tex_coord.resize(tri_strip_buffer_size);
}

void TerrainMaker::prepareData(std::int32_t new_steps,
                               std::int32_t new_increase,
                               float new_radius,
                               std::int32_t new_random_jump,
                               std::int32_t smoothness) {
    std::int32_t chunk_size = size / 2;
    steps = new_steps;
    increase = new_increase;
    radius = new_radius;
    random_jump = new_random_jump;
    std::int32_t buffersize = tri_strip_buffer_size;
    std::int32_t prep_size = size;
    std::int32_t prep_scale = scale;
    terrain.generate(
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
            Vertex v_i(j * prep_scale,
                       terrain.heightAt(j, i) /*SCALE*/,
                       i * prep_scale);
            vertices[index++] = v_i;
            TexCoord t_i((static_cast<float>(i % (chunk_size - 1))) /
                                 static_cast<float>(chunk_size - 1),
                         (static_cast<float>(j % (chunk_size - 1))) /
                                 static_cast<float>(chunk_size - 1));
            tex_coord[index_texture++] = t_i;
            Normal n_i(0, 0, 0);
            if (i == 0 && j == 0) {
            } else if (i == 0) {
            } else if (j == 0) {
            } else {
                n_i = toNormal(terrain.normalAt(j, i));
            }
            normals[index_normals++] = n_i;

            /************************************************************/
            /*	V_J -- N_J												*/
            /************************************************************/
            Vertex v_j(j * prep_scale,
                       terrain.heightAt(j, i + 1) /*SCALE*/,
                       (i + 1) * prep_scale);
            vertices[index++] = v_j;
            TexCoord t_j((static_cast<float>(i % (chunk_size - 1)) + 1) /
                                 static_cast<float>(chunk_size - 1),
                         (static_cast<float>(j % (chunk_size - 1))) /
                                 static_cast<float>(chunk_size - 1));
            tex_coord[index_texture++] = t_j;
            Normal n_j(0, 0, 0);
            if (i == prep_size - 2 && j == 0) {
            } else if (j == 0) {
            } else if (i == prep_size - 2) {
            } else {
                n_j = toNormal(terrain.normalAt(j, i + 1));
            }
            normals[index_normals++] = n_j;

            /************************************************************/
            /*	V_K -- N_K												*/
            /************************************************************/
            Vertex v_k((j + 1) * prep_scale,
                       terrain.heightAt(j + 1, i) /*SCALE*/,
                       (i)*prep_scale);
            vertices[index++] = v_k;
            TexCoord t_k((static_cast<float>(i % (chunk_size - 1))) /
                                 static_cast<float>(chunk_size - 1),
                         (static_cast<float>(j % (chunk_size - 1)) + 1) /
                                 static_cast<float>(chunk_size - 1));
            tex_coord[index_texture++] = t_k;
            Normal n_k(0, 0, 0);
            if (i == 0 && j == prep_size - 2) {
            } else if (i == 0) {
            } else if (j == prep_size - 2) {
            } else {
                n_k = toNormal(terrain.normalAt(j + 1, i));
            }
            normals[index_normals++] = n_k;

            /************************************************************/
            /*	V_X -- N_X	(SAME AS V_J/N_J)							*/
            /************************************************************/
            Vertex v_x(j * prep_scale,
                       terrain.heightAt(j, i + 1) /*SCALE*/,
                       (i + 1) * prep_scale);
            vertices[index++] = v_x;
            TexCoord t_x((static_cast<float>(i % (chunk_size - 1)) + 1) /
                                 static_cast<float>(chunk_size - 1),
                         (static_cast<float>(j % (chunk_size - 1))) /
                                 static_cast<float>(chunk_size - 1));
            tex_coord[index_texture++] = t_x;
            Normal n_x(0, 0, 0);
            if (i == prep_size - 2 && j == 0) {
            } else if (j == 0) {
            } else if (i == prep_size - 2) {
            } else {
                n_x = toNormal(terrain.normalAt(j, i + 1));
            }
            normals[index_normals++] = n_x;

            /************************************************************/
            /*	V_Y -- N_Y												*/
            /************************************************************/
            Vertex v_y((j + 1) * prep_scale,
                       terrain.heightAt(j + 1, i + 1) /*SCALE*/,
                       (i + 1) * prep_scale);
            vertices[index++] = v_y;
            TexCoord t_y((static_cast<float>(i % (chunk_size - 1)) + 1) /
                                 static_cast<float>(chunk_size - 1),
                         (static_cast<float>(j % (chunk_size - 1)) + 1) /
                                 static_cast<float>(chunk_size - 1));
            tex_coord[index_texture++] = t_y;
            Normal n_y(0, 0, 0);
            if (i == prep_size - 2 && j == prep_size - 2) {
            } else if (i == prep_size - 2) {
            } else if (j == prep_size - 2) {
            } else {
                n_y = toNormal(terrain.normalAt(j + 1, i + 1));
            }
            normals[index_normals++] = n_y;

            /************************************************************/
            /*	V_Z -- N_Z												*/
            /************************************************************/
            Vertex v_z((j + 1) * prep_scale,
                       terrain.heightAt(j + 1, i) /*SCALE*/,
                       (i)*prep_scale);
            vertices[index++] = v_z;
            TexCoord t_z((static_cast<float>(i % (chunk_size - 1))) /
                                 static_cast<float>(chunk_size - 1),
                         (static_cast<float>(j % (chunk_size - 1)) + 1) /
                                 static_cast<float>(chunk_size - 1));
            tex_coord[index_texture++] = t_z;
            Normal n_z(0, 0, 0);
            if (i == 0 && j == prep_size - 2) {
            } else if (i == 0) {
            } else if (j == prep_size - 2) {
            } else {
                n_z = toNormal(terrain.normalAt(j + 1, i));
            }
            normals[index_normals++] = n_z;
        }
    }

    mesh_dirty = true;
}

void TerrainMaker::toggleWireframe() { wireframe_active = !wireframe_active; }

Normal TerrainMaker::getTriangleNormal(float x, float z) {
    return toNormal(terrain.triangleNormalAt(static_cast<std::int32_t>(x),
                                             static_cast<std::int32_t>(z)));
}

Normal TerrainMaker::getNormalAt(float x, float z) {
    return toNormal(terrain.normalAtWorld(x, z));
}

float TerrainMaker::getHeightAt(float x, float z) {
    return terrain.heightAtWorld(x, z);
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
    std::vector<vulkan_graphix::TerrainGridCell> const cells =
            terrain.makeCrater(impact_x, impact_z, blast_size);

    std::int32_t const cells_per_row = size - 1;
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

    for (vulkan_graphix::TerrainGridCell const& cell : cells) {
        float const height =
                static_cast<float>(terrain.heightAt(cell.x, cell.z));
        for (std::int32_t index : slots_for(cell.x, cell.z)) {
            if (index >= 0) {
                vertices[index].coord_y = height;
            }
        }
    }

    for (vulkan_graphix::TerrainGridCell const& cell : cells) {
        Normal const normal = toNormal(terrain.normalAt(cell.x, cell.z));
        for (std::int32_t index : slots_for(cell.x, cell.z)) {
            if (index >= 0) {
                normals[index] = normal;
            }
        }
    }
    mesh_dirty = true;
}

/************************************************************************/
/*	Debugging MessageBox functions										*/
/************************************************************************/
void TerrainMaker::stdMessageBox(const std::string& output) {}

void TerrainMaker::errorMessageBox(const std::string& output) {}

#include "vulkan_earth/TerrainMaker.h"
#include <GL/glx.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include "math.h"
#include "vulkan_earth/Normal.h"
#include "vulkan_earth/Shader.h"
#include "vulkan_earth/TexCoord.h"
#include "vulkan_earth/Vertex.h"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace {
Normal toNormal(vulkan_graphix::Math::Vec3<float> const& normal) {
    return Normal(normal.x, normal.y, normal.z);
}
}  // namespace

TerrainMaker::TerrainMaker(int i_scale, int i_size)
        : terrain(i_size, i_scale) {
    srand(time(nullptr));
    scale = i_scale;
    size = i_size;
    total_vertices = size * size;
    tri_strip_buffer_size = (size - 1) * (size - 1) * 6;
    initData();

    shader = new Shader();
    shader->init("VertexShader.vs", "FragmentShader.vs");
    color_texture = loadTexture("Rocky.raw", 2048, 2048);
    normal_texture = loadTexture("bumpMap.raw", 256, 256);

    rotation_angle = 0.0;
    vbo_qualify = nullptr;
    verifyVBOs();
    wireframe_active = false;
}

TerrainMaker::~TerrainMaker() {
    delete vbo_qualify;
    delete shader;
    pgl_delete_buffers_arb(1, &vertex_vbo_id);
    pgl_delete_buffers_arb(1, &normal_vbo_id);
    pgl_delete_buffers_arb(1, &texture_vbo_id);
    glDeleteTextures(1, &color_texture);
    glDeleteTextures(1, &normal_texture);
}

std::int32_t TerrainMaker::getScale() { return scale; }
std::int32_t TerrainMaker::getActualSize() { return (size) * (scale); }

std::uint32_t TerrainMaker::selectTexture(const std::string& tex) {
    glDeleteTextures(1, &color_texture);

    if (tex == "Rock")
        return loadTexture("Rocky.raw", 2048, 2048);
    else if (tex == "Snow")
        return loadTexture("Snowy.raw", 2048, 2048);
    else if (tex == "Ice")
        return loadTexture("Icy.raw", 2048, 2048);
    else if (tex == "Mars")
        return loadTexture("RedPlanet.raw", 2048, 2048);
    else if (tex == "Desert")
        return loadTexture("Desert.raw", 2048, 2048);
    else if (tex == "Lava")
        return loadTexture("LavaRock.raw", 2048, 2048);
    else
        return 0;
}

std::uint32_t TerrainMaker::loadTexture(const char* filename,
                                        int width,
                                        int height) {
    std::uint32_t texture;
    std::ifstream file(filename, std::ios::binary);
    if (!file) return 0;
    std::vector<unsigned char> data(width * height * 3);
    file.read(reinterpret_cast<char*>(data.data()), data.size());
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 width,
                 height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 data.data());
    return texture;
}

void TerrainMaker::draw() {
    int draw_size = size;
    int draw_scale = scale;
    int buffersize = tri_strip_buffer_size;
    glEnable(GL_COLOR_MATERIAL);

    if (wireframe_active) {
        glColor4f(0, 0, 0, .75);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnableClientState(GL_VERTEX_ARRAY);
        pgl_bind_buffer_arb(GL_ARRAY_BUFFER_ARB, vertex_vbo_id);
        glVertexPointer(3, GL_FLOAT, 0, nullptr);
        glDrawArrays(GL_TRIANGLES, 0, buffersize);
        glDisableClientState(GL_VERTEX_ARRAY);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glColor4f(1, 1, 1, .75);
        for (int i = 0; i < 255; i += 4) {
            for (int j = 0; j < 255; j += 4) {
                glBegin(GL_LINES);
                /*glVertex3f(i*draw_scale,terrain.heightAt(i, j),j*draw_scale);
                glVertex3f(	i*draw_scale+500*normals[(i*255+j)*6].compoX,
                            terrain.heightAt(i,
                j)+500*normals[(i*255+j)*6].compoY,
                            j*draw_scale+500*normals[(i*255+j)*6].compoZ);*/
                glVertex3f(i * draw_scale,
                           terrain.heightAt(i, j),
                           j * draw_scale);
                glVertex3f(i * draw_scale + 500 * getNormalAt(i * draw_scale,
                                                              j * draw_scale)
                                                            .compo_x,
                           terrain.heightAt(i, j) +
                                   500 * getNormalAt(i * draw_scale,
                                                     j * draw_scale)
                                                   .compo_y,
                           j * draw_scale + 500 * getNormalAt(i * draw_scale,
                                                              j * draw_scale)
                                                            .compo_z);
                glEnd();
            }
        }
    } else {
        shader->bind();
        glEnable(GL_LIGHTING);  // NOT PART OF SHADER CODE

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        int texture_location =
                glGetUniformLocation(shader->id(), "color_texture");
        glUniform1i(texture_location, 0);
        glBindTexture(GL_TEXTURE_2D, color_texture);

        glActiveTexture(GL_TEXTURE1);
        glEnable(GL_TEXTURE_2D);
        int normal_location =
                glGetUniformLocation(shader->id(), "normal_texture");
        glUniform1i(normal_location, 1);
        glBindTexture(GL_TEXTURE_2D, normal_texture);

        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        pgl_bind_buffer_arb(GL_ARRAY_BUFFER_ARB, vertex_vbo_id);
        glVertexPointer(3, GL_FLOAT, 0, nullptr);
        glNormalPointer(GL_FLOAT,
                        0,
                        reinterpret_cast<void*>(buffersize * sizeof(Vertex)));
        glTexCoordPointer(
                2,
                GL_FLOAT,
                0,
                reinterpret_cast<void*>(buffersize *
                                        (sizeof(Vertex) + sizeof(Normal))));
        glDrawArrays(GL_TRIANGLES, 0, buffersize);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        glDisable(GL_LIGHTING);  // NOT PART OF SHADER CODE
        shader->unbind();
    }
    glDisable(GL_COLOR_MATERIAL);
}

void TerrainMaker::initData() {
    vertices.resize(tri_strip_buffer_size);
    normals.resize(tri_strip_buffer_size);
    tex_coord.resize(tri_strip_buffer_size);
    material_specular = {1.0, 1.0, 1.0, 1.0};
    material_shininess = {10000.0};
    material_diffuse = {0.0, 1.0, 0.0, 1.0};
}

void TerrainMaker::prepareData(int new_steps,
                               int new_increase,
                               float new_radius,
                               int new_random_jump,
                               int smoothness) {
    int chunk_size = size / 2;
    steps = new_steps;
    increase = new_increase;
    radius = new_radius;
    random_jump = new_random_jump;
    int buffersize = tri_strip_buffer_size;
    int prep_size = size;
    int prep_scale = scale;
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
    int index = 0;
    int index_normals = 0;
    int index_texture = 0;
    for (int i = 0; i < prep_size - 1; i++) {
        for (int j = 0; j < prep_size - 1; j++) {
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

    pgl_gen_buffers_arb(1, &vertex_vbo_id);  // Create VBO for Vertices
    pgl_bind_buffer_arb(GL_ARRAY_BUFFER_ARB, vertex_vbo_id);
    pgl_buffer_data_arb(
            GL_ARRAY_BUFFER_ARB,
            buffersize * (sizeof(Vertex) + sizeof(Normal) + sizeof(TexCoord)),
            nullptr,
            GL_DYNAMIC_DRAW_ARB);

    pgl_buffer_sub_data_arb(GL_ARRAY_BUFFER_ARB,
                            0,
                            buffersize * sizeof(Vertex),
                            vertices.data());

    pgl_buffer_sub_data_arb(GL_ARRAY_BUFFER_ARB,
                            buffersize * sizeof(Vertex),
                            buffersize * sizeof(Normal),
                            normals.data());

    pgl_buffer_sub_data_arb(GL_ARRAY_BUFFER_ARB,
                            buffersize * (sizeof(Vertex) + sizeof(Normal)),
                            buffersize * sizeof(TexCoord),
                            tex_coord.data());
}

void TerrainMaker::verifyVBOs() {
    delete vbo_qualify;
    vbo_qualify = new VBOQualifer();
    vbo_qualify->establishIfQualified();
    if (vbo_qualify->getQualified()) {
        if (vbo_qualify->isExtensionSupported("GL_ARB_vertex_buffer_object")) {
            pgl_gen_buffers_arb = reinterpret_cast<PFNGLGENBUFFERSARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const std::uint8_t*>(
                            "glGenBuffersARB")));
            pgl_bind_buffer_arb = reinterpret_cast<PFNGLBINDBUFFERARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const std::uint8_t*>(
                            "glBindBufferARB")));
            pgl_buffer_data_arb = reinterpret_cast<PFNGLBUFFERDATAARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const std::uint8_t*>(
                            "glBufferDataARB")));
            pgl_buffer_sub_data_arb =
                    reinterpret_cast<PFNGLBUFFERSUBDATAARBPROC>(
                            glXGetProcAddress(
                                    reinterpret_cast<const std::uint8_t*>(
                                            "glBufferSubDataARB")));
            pgl_delete_buffers_arb =
                    reinterpret_cast<PFNGLDELETEBUFFERSARBPROC>(
                            glXGetProcAddress(
                                    reinterpret_cast<const std::uint8_t*>(
                                            "glDeleteBuffersARB")));
            pgl_get_buffer_parameteriv_arb =
                    reinterpret_cast<PFNGLGETBUFFERPARAMETERIVARBPROC>(
                            glXGetProcAddress(
                                    reinterpret_cast<const std::uint8_t*>(
                                            "glGetBufferParameterivARB")));
            pgl_map_buffer_arb = reinterpret_cast<PFNGLMAPBUFFERARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const std::uint8_t*>(
                            "glMapBufferARB")));
            pgl_unmap_buffer_arb = reinterpret_cast<PFNGLUNMAPBUFFERARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const std::uint8_t*>(
                            "glUnmapBufferARB")));
            if (pgl_gen_buffers_arb && pgl_bind_buffer_arb &&
                pgl_buffer_data_arb && pgl_buffer_sub_data_arb &&
                pgl_delete_buffers_arb && pgl_get_buffer_parameteriv_arb &&
                pgl_map_buffer_arb && pgl_unmap_buffer_arb) {
            } else {
                stdMessageBox(
                        "Pointers to Buffer Functions Failed to be Obtained");
                exit(0);
            }
        } else {
            stdMessageBox("GL_ARB_vertex_buffer_object IS NOT Supported");
            exit(0);
        }
    } else {
        stdMessageBox("VBOs Creation Failed");
        exit(0);
    }
}

void TerrainMaker::toggleWireframe() { wireframe_active = !wireframe_active; }

Normal TerrainMaker::getTriangleNormal(float x, float z) {
    return toNormal(terrain.triangleNormalAt(static_cast<int>(x),
                                             static_cast<int>(z)));
}

Normal TerrainMaker::getNormalAt(float x, float z) {
    return toNormal(terrain.normalAtWorld(x, z));
}

float TerrainMaker::getHeightAt(float x, float z) {
    return terrain.heightAtWorld(x, z);
}

// The crater's height math is TerrainGenerator::makeCrater(); this only
// re-uploads the affected grid vertices into the VBO. prepareData() lays
// the mesh out as six triangle-list slots per grid cell (row z, column x),
// so each grid vertex is repeated in up to six slots across the cells
// around it - the slot choice (including skipping row/column 0's
// neighbors via `> 0`) is TerrainMaker's own. Unlike the original, a slot
// outside the (size - 1) x (size - 1) cell grid is skipped rather than
// written past the end of the buffer.
void TerrainMaker::makeCrater(float impact_x,
                              float impact_z,
                              float blast_size) {
    std::vector<vulkan_graphix::TerrainGridCell> const cells =
            terrain.makeCrater(impact_x, impact_z, blast_size);
    Vertex* buffer_ptr = static_cast<Vertex*>(
            pgl_map_buffer_arb(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE));

    int const cells_per_row = size - 1;
    auto slot = [&](int row, int col, int corner) -> int {
        if (row < 0 || col < 0 || row >= cells_per_row ||
            col >= cells_per_row) {
            return -1;
        }
        return (row * cells_per_row + col) * 6 + corner;
    };
    // Every slot grid vertex (i, j) occupies, per TerrainMaker's layout.
    auto slots_for = [&](int grid_x, int grid_z) {
        std::vector<int> slots = {slot(grid_z, grid_x, 0)};
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
        for (int index : slots_for(cell.x, cell.z)) {
            if (index >= 0) {
                buffer_ptr[index].coord_y = height;  // VBO
            }
        }
    }

    int const normal_offset = tri_strip_buffer_size;
    for (vulkan_graphix::TerrainGridCell const& cell : cells) {
        Normal const normal = toNormal(terrain.normalAt(cell.x, cell.z));
        for (int index : slots_for(cell.x, cell.z)) {
            if (index >= 0) {
                buffer_ptr[normal_offset + index].coord_x = normal.compo_x;
                buffer_ptr[normal_offset + index].coord_y = normal.compo_y;
                buffer_ptr[normal_offset + index].coord_z = normal.compo_z;
            }
        }
    }
    pgl_unmap_buffer_arb(GL_ARRAY_BUFFER_ARB);
}

/************************************************************************/
/*	Debugging MessageBox functions										*/
/************************************************************************/
void TerrainMaker::stdMessageBox(const std::string& output) {}

void TerrainMaker::errorMessageBox(const std::string& output) {}

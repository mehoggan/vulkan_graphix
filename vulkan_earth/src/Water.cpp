#include "vulkan_earth/Water.h"
#include <cstdint>
#include <cstdlib>
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
// #include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace render = vulkan_earth::render;

Water::Water() = default;

Water::Water(std::int32_t new_scale, std::int32_t new_size) {
    srand(time(nullptr));
    timer = 0.0;
    scale = new_scale;
    size = new_size;
    total_vertices = size * size;
    tri_strip_buffer_size = (size - 1) * (size - 1) * 6;
    initData();
    prepTerrain();
    // GL_LINEAR filtering, GL_REPEAT wrapping. (The original also loaded
    // bumpMap.raw as a normal map; its water shader never used it.)
    color_texture = render::Renderer::instance().loadRawTexture(
            "Water.raw", 1024, 1024, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    prepareData();
}

Water::~Water() {
    if (surfaceheight != nullptr) {
        for (std::int32_t i = 0; i < size; i++) {
            delete surfaceheight[i];
        }
        delete surfaceheight;
    }
}

std::int32_t Water::getScale() { return scale; }
std::int32_t Water::getActualSize() { return (size) * (scale); }

void Water::initData() {
    vertices.resize(tri_strip_buffer_size);
    normals.resize(tri_strip_buffer_size);
    tex_coord.resize(tri_strip_buffer_size);
}

void Water::draw(render::RenderContext& context) {
    if (mesh) {
        context.drawMesh(*mesh,
                         render::PipelineId::Water,
                         color_texture.get(),
                         render::Mat4(1.0f),
                         render::Vec4(timer, 0.0f, 0.0f, 0.0f));
    }
    timer += 0.002 * 3.14159265;
    if (timer >= 2 * 3.14159265) timer = 0.0;
}

void Water::calcAverageofSixNormals(Vertex* v_0,
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
                                    Normal* n) {
    float u_1_x = x1 - v_0->coord_x;
    float u_1_y = y1 - v_0->coord_y;
    float u_1_z = z1 - v_0->coord_z;
    float u_2_x = x2 - v_0->coord_x;
    float u_2_y = y2 - v_0->coord_y;
    float u_2_z = z2 - v_0->coord_z;
    float u_3_x = x3 - v_0->coord_x;
    float u_3_y = y3 - v_0->coord_y;
    float u_3_z = z3 - v_0->coord_z;
    float u_4_x = x4 - v_0->coord_x;
    float u_4_y = y4 - v_0->coord_y;
    float u_4_z = z4 - v_0->coord_z;
    float u_5_x = x5 - v_0->coord_x;
    float u_5_y = y5 - v_0->coord_y;
    float u_5_z = z5 - v_0->coord_z;
    float u_6_x = x6 - v_0->coord_x;
    float u_6_y = y6 - v_0->coord_y;
    float u_6_z = z6 - v_0->coord_z;
    n->compo_x += u_6_y * u_1_z - u_1_y * u_6_z;
    n->compo_y += u_6_z * u_1_x - u_6_x * u_1_z;
    n->compo_z += u_6_x * u_1_y - u_1_x * u_6_y;
    n->compo_x += u_1_y * u_2_z - u_2_y * u_1_z;
    n->compo_y += u_1_z * u_2_x - u_1_x * u_2_z;
    n->compo_z += u_1_x * u_2_y - u_2_x * u_1_y;
    n->compo_x += u_2_y * u_3_z - u_3_y * u_2_z;
    n->compo_y += u_2_z * u_3_x - u_2_x * u_3_z;
    n->compo_z += u_2_x * u_3_y - u_3_x * u_2_y;
    n->compo_x += u_3_y * u_4_z - u_4_y * u_3_z;
    n->compo_y += u_3_z * u_4_x - u_3_x * u_4_z;
    n->compo_z += u_3_x * u_4_y - u_4_x * u_3_y;
    n->compo_x += u_4_y * u_5_z - u_5_y * u_4_z;
    n->compo_y += u_4_z * u_5_x - u_4_x * u_5_z;
    n->compo_z += u_4_x * u_5_y - u_5_x * u_4_y;
    n->compo_x += u_5_y * u_6_z - u_6_y * u_5_z;
    n->compo_y += u_5_z * u_6_x - u_5_x * u_6_z;
    n->compo_z += u_5_x * u_6_y - u_6_x * u_5_y;
    n->compo_x /= 6;
    n->compo_y /= 6;
    n->compo_z /= 6;
    float magnitude =
            sqrt((n->compo_x * n->compo_x) + (n->compo_y * n->compo_y) +
                 (n->compo_z * n->compo_z));
    n->compo_x /= magnitude;
    n->compo_y /= magnitude;
    n->compo_z /= magnitude;
}

void Water::prepTerrain() {
    surfaceheight = new std::int32_t*[size];
    for (std::int32_t i = 0; i < size; i++) {
        surfaceheight[i] = new std::int32_t[size];
    }
    for (std::int32_t y = 0; y < size; y++) {
        for (std::int32_t x = 0; x < size; x++) {
            surfaceheight[x][y] = -5 * 100;
        }
    }
}

void Water::prepareData() {
    std::int32_t buffersize = tri_strip_buffer_size;
    std::int32_t prep_size = size;
    std::int32_t prep_scale = scale;

    //
    // 				v_k
    //	v_i			v_z
    //		x-----x
    //		|    /|
    //		|  /  |
    //		|/    |
    //		x-----x
    //	v_j			v_y
    //	v_x
    std::int32_t index = 0;
    std::int32_t index_normals = 0;
    std::int32_t index_texture = 0;
    for (std::int32_t i = 0; i < prep_size - 1; i++) {
        for (std::int32_t j = 0; j < prep_size - 1; j++) {
            /************************************************************/
            /*	V_I -- N_I		                            */
            /************************************************************/
            Vertex v_i(j * prep_scale,
                       surfaceheight[i][j] /*SCALE*/,
                       i * prep_scale);
            vertices[index++] = v_i;
            TexCoord t_i(i / (static_cast<float>(prep_size) - 1),
                         (j) / (static_cast<float>(prep_size) - 1));
            tex_coord[index_texture++] = t_i;
            Normal n_i(0, 0, 0);
            if (i == 0 && j == 0) {
                calcAverageofSixNormals(&v_i,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        &n_i);
            } else if (i == 0) {
                calcAverageofSixNormals(&v_i,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        static_cast<float>(j - 1),
                                        surfaceheight[i][j - 1],
                                        static_cast<float>(i),
                                        static_cast<float>(j - 1),
                                        surfaceheight[i + 1][j - 1],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        &n_i);
            } else if (j == 0) {
                calcAverageofSixNormals(&v_i,
                                        static_cast<float>(j + 1),
                                        surfaceheight[i - 1][j + 1],
                                        static_cast<float>(i - 1),
                                        static_cast<float>(j),
                                        surfaceheight[i - 1][j],
                                        static_cast<float>(i - 1),
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        &n_i);
            } else {
                calcAverageofSixNormals(&v_i,
                                        static_cast<float>(j + 1),
                                        surfaceheight[i - 1][j + 1],
                                        static_cast<float>(i - 1),
                                        static_cast<float>(j),
                                        surfaceheight[i - 1][j],
                                        static_cast<float>(i - 1),
                                        static_cast<float>(j - 1),
                                        surfaceheight[i][j - 1],
                                        static_cast<float>(i),
                                        static_cast<float>(j - 1),
                                        surfaceheight[i + 1][j - 1],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        &n_i);
            }
            normals[index_normals++] = n_i;
            /************************************************************/
            /*	V_J -- N_J		                 	    */
            /************************************************************/
            Vertex v_j(j * prep_scale,
                       surfaceheight[i + 1][j] /*SCALE*/,
                       (i + 1) * prep_scale);
            vertices[index++] = v_j;
            TexCoord t_j((i + 1) / (static_cast<float>(prep_size) - 1),
                         (j) / (static_cast<float>(prep_size) - 1));
            tex_coord[index_texture++] = t_j;
            Normal n_j(0, 0, 0);
            if (i == prep_size - 2 && j == 0) {
                calcAverageofSixNormals(&v_j,
                                        static_cast<float>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        v_j.coord_x,
                                        v_j.coord_y,
                                        v_j.coord_z,
                                        v_j.coord_x,
                                        v_j.coord_y,
                                        v_j.coord_z,
                                        v_j.coord_x,
                                        v_j.coord_y,
                                        v_j.coord_z,
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        &n_j);
            } else if (j == 0) {
                calcAverageofSixNormals(&v_j,
                                        static_cast<float>(j) + 1,
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        v_j.coord_x,
                                        v_j.coord_y,
                                        v_j.coord_z,
                                        v_j.coord_x,
                                        v_j.coord_y,
                                        v_j.coord_z,
                                        static_cast<float>(j),
                                        surfaceheight[i + 2][j],
                                        static_cast<float>(i + 2),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        &n_j);
            } else if (i == prep_size - 2) {
                calcAverageofSixNormals(&v_j,
                                        static_cast<float>(j) + 1,
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        static_cast<float>(j - 1),
                                        surfaceheight[i + 1][j - 1],
                                        static_cast<float>(i + 1),
                                        v_j.coord_x,
                                        v_j.coord_y,
                                        v_j.coord_z,
                                        v_j.coord_x,
                                        v_j.coord_y,
                                        v_j.coord_z,
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        &n_j);
            } else {
                calcAverageofSixNormals(&v_j,
                                        static_cast<float>(j) + 1,
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        static_cast<float>(j - 1),
                                        surfaceheight[i + 1][j - 1],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j - 1),
                                        surfaceheight[i + 2][j - 1],
                                        static_cast<float>(i + 2),
                                        static_cast<float>(j),
                                        surfaceheight[i + 2][j],
                                        static_cast<float>(i + 2),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        &n_j);
            }
            normals[index_normals++] = n_j;
            /************************************************************/
            /*	V_K -- N_K					    */
            /************************************************************/
            Vertex v_k((j + 1) * prep_scale,
                       surfaceheight[i][j + 1] /*SCALE*/,
                       (i)*prep_scale);
            vertices[index++] = v_k;
            TexCoord t_k(i / (static_cast<float>(prep_size) - 1),
                         (j + 1) / (static_cast<float>(prep_size) - 1));
            tex_coord[index_texture++] = t_k;
            Normal n_k(0, 0, 0);
            if (i == 0 && j == prep_size - 2) {
                calcAverageofSixNormals(&v_k,
                                        v_k.coord_x,
                                        v_k.coord_y,
                                        v_k.coord_z,
                                        v_k.coord_x,
                                        v_k.coord_y,
                                        v_k.coord_z,
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        v_k.coord_x,
                                        v_k.coord_y,
                                        v_k.coord_z,
                                        &n_k);
            } else if (i == 0) {
                calcAverageofSixNormals(&v_k,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 2),
                                        surfaceheight[i][j + 2],
                                        static_cast<float>(i),
                                        &n_k);
            } else if (j == prep_size - 2) {
                calcAverageofSixNormals(&v_k,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        static_cast<float>(j + 1),
                                        surfaceheight[i - 1][j + 1],
                                        static_cast<float>(i - 1),
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        &n_k);
            } else {
                calcAverageofSixNormals(&v_k,
                                        static_cast<float>(j + 2),
                                        surfaceheight[i - 1][j + 2],
                                        static_cast<float>(i - 1),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i - 1][j + 1],
                                        static_cast<float>(i - 1),
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 2),
                                        surfaceheight[i][j + 2],
                                        static_cast<float>(i),
                                        &n_k);
            }
            normals[index_normals++] = n_k;
            /************************************************************/
            /*	V_X -- N_X	(SAME AS V_J/N_J)	            */
            /************************************************************/
            Vertex v_x(j * prep_scale,
                       surfaceheight[i + 1][j] /*SCALE*/,
                       (i + 1) * prep_scale);
            vertices[index++] = v_x;
            TexCoord t_x((i + 1) / (static_cast<float>(prep_size) - 1),
                         (j) / (static_cast<float>(prep_size) - 1));
            tex_coord[index_texture++] = t_x;
            Normal n_x(0, 0, 0);
            if (i == prep_size - 2 && j == 0) {
                calcAverageofSixNormals(&v_x,
                                        static_cast<float>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        v_x.coord_x,
                                        v_x.coord_y,
                                        v_x.coord_z,
                                        v_x.coord_x,
                                        v_x.coord_y,
                                        v_x.coord_z,
                                        v_x.coord_x,
                                        v_x.coord_y,
                                        v_x.coord_z,
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        &n_x);
            } else if (j == 0) {
                calcAverageofSixNormals(&v_x,
                                        static_cast<float>(j) + 1,
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        v_x.coord_x,
                                        v_x.coord_y,
                                        v_x.coord_z,
                                        v_x.coord_x,
                                        v_x.coord_y,
                                        v_x.coord_z,
                                        static_cast<float>(j),
                                        surfaceheight[i + 2][j],
                                        static_cast<float>(i + 2),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        &n_x);
            } else if (i == prep_size - 2) {
                calcAverageofSixNormals(&v_x,
                                        static_cast<float>(j) + 1,
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        static_cast<float>(j - 1),
                                        surfaceheight[i + 1][j - 1],
                                        static_cast<float>(i + 1),
                                        v_x.coord_x,
                                        v_x.coord_y,
                                        v_x.coord_z,
                                        v_x.coord_x,
                                        v_x.coord_y,
                                        v_x.coord_z,
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        &n_x);
            } else {
                calcAverageofSixNormals(&v_x,
                                        static_cast<float>(j) + 1,
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        static_cast<float>(j - 1),
                                        surfaceheight[i + 1][j - 1],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j - 1),
                                        surfaceheight[i + 2][j - 1],
                                        static_cast<float>(i + 2),
                                        static_cast<float>(j),
                                        surfaceheight[i + 2][j],
                                        static_cast<float>(i + 2),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        &n_x);
            }
            normals[index_normals++] = n_x;

            /************************************************************/
            /*	V_Y -- N_Y					    */
            /************************************************************/
            Vertex v_y((j + 1) * prep_scale,
                       surfaceheight[i + 1][j + 1] /*SCALE*/,
                       (i + 1) * prep_scale);
            vertices[index++] = v_y;
            TexCoord t_y((i + 1) / (static_cast<float>(prep_size) - 1),
                         (j + 1) / (static_cast<float>(prep_size) - 1));
            tex_coord[index_texture++] = t_y;
            Normal n_y(0, 0, 0);
            if (i == prep_size - 2 && j == prep_size - 2) {
                calcAverageofSixNormals(&v_y,
                                        v_y.coord_x,
                                        v_y.coord_y,
                                        v_y.coord_z,
                                        static_cast<float>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        v_y.coord_x,
                                        v_y.coord_y,
                                        v_y.coord_z,
                                        v_y.coord_x,
                                        v_y.coord_y,
                                        v_y.coord_z,
                                        v_y.coord_x,
                                        v_y.coord_y,
                                        v_y.coord_z,
                                        &n_y);
            } else if (i == prep_size - 2) {
                calcAverageofSixNormals(&v_y,
                                        static_cast<float>(j + 2),
                                        surfaceheight[i][j + 2],
                                        static_cast<float>(i),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        v_y.coord_x,
                                        v_y.coord_y,
                                        v_y.coord_z,
                                        v_y.coord_x,
                                        v_y.coord_y,
                                        v_y.coord_z,
                                        static_cast<float>(j + 2),
                                        surfaceheight[i + 1][j + 2],
                                        static_cast<float>(i + 1),
                                        &n_y);
            } else if (j == prep_size - 2) {
                calcAverageofSixNormals(&v_y,
                                        v_y.coord_x,
                                        v_y.coord_y,
                                        v_y.coord_z,
                                        static_cast<float>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j),
                                        surfaceheight[i + 2][j],
                                        static_cast<float>(i + 2),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 2][j + 1],
                                        static_cast<float>(i + 2),
                                        v_y.coord_x,
                                        v_y.coord_y,
                                        v_y.coord_z,
                                        &n_y);
            } else {
                calcAverageofSixNormals(&v_y,
                                        static_cast<float>(j + 2),
                                        surfaceheight[i][j + 2],
                                        static_cast<float>(i),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j),
                                        surfaceheight[i + 2][j],
                                        static_cast<float>(i + 2),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 2][j + 1],
                                        static_cast<float>(i + 2),
                                        static_cast<float>(j + 2),
                                        surfaceheight[i + 1][j + 2],
                                        static_cast<float>(i + 1),
                                        &n_y);
            }
            normals[index_normals++] = n_y;
            /************************************************************/
            /*	V_Z -- N_Z					    */
            /************************************************************/
            Vertex v_z((j + 1) * prep_scale,
                       surfaceheight[i][j + 1] /*SCALE*/,
                       (i)*prep_scale);
            vertices[index++] = v_z;
            TexCoord t_z(i / (static_cast<float>(prep_size) - 1),
                         (j + 1) / (static_cast<float>(prep_size) - 1));
            tex_coord[index_texture++] = t_z;
            Normal n_z(0, 0, 0);
            if (i == 0 && j == prep_size - 2) {
                calcAverageofSixNormals(&v_z,
                                        v_z.coord_x,
                                        v_z.coord_y,
                                        v_z.coord_z,
                                        v_z.coord_x,
                                        v_z.coord_y,
                                        v_z.coord_z,
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        v_z.coord_x,
                                        v_z.coord_y,
                                        v_z.coord_z,
                                        &n_z);
            } else if (i == 0) {
                calcAverageofSixNormals(&v_z,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 2),
                                        surfaceheight[i][j + 2],
                                        static_cast<float>(i),
                                        &n_z);
            } else if (j == prep_size - 2) {
                calcAverageofSixNormals(&v_z,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        static_cast<float>(j + 1),
                                        surfaceheight[i - 1][j + 1],
                                        static_cast<float>(i - 1),
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        &n_z);
            } else {
                calcAverageofSixNormals(&v_z,
                                        static_cast<float>(j + 2),
                                        surfaceheight[i - 1][j + 2],
                                        static_cast<float>(i - 1),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i - 1][j + 1],
                                        static_cast<float>(i - 1),
                                        static_cast<float>(j),
                                        surfaceheight[i][j],
                                        static_cast<float>(i),
                                        static_cast<float>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<float>(i + 1),
                                        static_cast<float>(j + 2),
                                        surfaceheight[i][j + 2],
                                        static_cast<float>(i),
                                        &n_z);
            }
            normals[index_normals++] = n_z;
        }
    }
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
}

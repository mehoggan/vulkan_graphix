#include "TerrainMaker.h"
#include <GL/glx.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include "Normal.h"
#include "Shader.h"
#include "TexCoord.h"
#include "Vertex.h"
#include "macro_crtdbg.h"
#include "math.h"

using namespace std;

TerrainMaker::TerrainMaker() = default;

TerrainMaker::TerrainMaker(int i_scale, int i_size) {
    srand(time(nullptr));
    scale = i_scale;
    size = i_size;
    total_vertices = size * size;
    tri_strip_buffer_size = (size - 1) * (size - 1) * 6;
    prepTerrain();
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
    if (th) {
        for (int i = 0; i < size; i++) {
            delete th[i];
        }
        delete th;
    }
    delete vbo_qualify;
    delete shader;
    pgl_delete_buffers_arb(1, &vertex_vbo_id);
    pgl_delete_buffers_arb(1, &normal_vbo_id);
    pgl_delete_buffers_arb(1, &texture_vbo_id);
    glDeleteTextures(1, &color_texture);
    glDeleteTextures(1, &normal_texture);
}

GLint TerrainMaker::getScale() { return scale; }
GLint TerrainMaker::getActualSize() { return (size) * (scale); }

GLuint TerrainMaker::selectTexture(const std::string& tex) {
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

GLuint TerrainMaker::loadTexture(const char* filename, int width, int height) {
    GLuint texture;
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
                /*glVertex3f(i*draw_scale,th[j][i],j*draw_scale);
                glVertex3f(	i*draw_scale+500*normals[(i*255+j)*6].compoX,
                            th[j][i]+500*normals[(i*255+j)*6].compoY,
                            j*draw_scale+500*normals[(i*255+j)*6].compoZ);*/
                glVertex3f(i * draw_scale, th[j][i], j * draw_scale);
                glVertex3f(i * draw_scale + 500 * getNormalAt(i * draw_scale,
                                                              j * draw_scale)
                                                            .compo_x,
                           th[j][i] + 500 * getNormalAt(i * draw_scale,
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

void TerrainMaker::smoothShadeNormal(int x, int z, Normal* n) {
    calcNormal(x, z - 1, 0, n);
    Normal n0(n->compo_x, n->compo_y, n->compo_z);
    calcNormal(x, z, 0, n);
    Normal n1(n->compo_x, n->compo_y, n->compo_z);
    calcNormal(x, z, 1, n);
    Normal n2(n->compo_x, n->compo_y, n->compo_z);
    calcNormal(x + 1, z + 1, 1, n);
    Normal n3(n->compo_x, n->compo_y, n->compo_z);
    calcNormal(x + 1, z, 1, n);
    Normal n4(n->compo_x, n->compo_y, n->compo_z);
    calcNormal(x + 1, z, 0, n);
    Normal n5(n->compo_x, n->compo_y, n->compo_z);

    n->compo_x = (n0.compo_x + n1.compo_x + n2.compo_x + n3.compo_x +
                  n4.compo_x + n5.compo_x) /
                 6;
    n->compo_y = (n0.compo_y + n1.compo_y + n2.compo_y + n3.compo_y +
                  n4.compo_y + n5.compo_y) /
                 6;
    n->compo_z = (n0.compo_z + n1.compo_z + n2.compo_z + n3.compo_z +
                  n4.compo_z + n5.compo_z) /
                 6;
}

void TerrainMaker::calcNormal(int x, int z, int flag, Normal* n) {
    float v1[3], v2[3];
    bool can_calculate = true;
    if (flag == 1) {
        if (((x - 1) >= 0) && ((z - 1) >= 0) && (x < size) && (z < size)) {
            v1[0] = ((x - 1) * scale) - (x * scale);
            v1[1] = (th[x - 1][z - 1]) - th[x][z];
            v1[2] = ((z - 1) * scale) - (z * scale);

            v2[0] = ((x - 1) * scale) - (x * scale);
            v2[1] = (th[x - 1][z]) - th[x][z];
            v2[2] = 0;
        } else {
            can_calculate = false;
        }
    } else {
        if ((((x - 1) >= 0) && (x < size)) && ((z + 1) < size) && (z > 0)) {
            v1[0] = ((x - 1) * scale) - (x * scale);
            v1[1] = (th[x - 1][z]) - th[x][z];
            v1[2] = 0;

            v2[0] = 0;
            v2[1] = (th[x][z + 1]) - th[x][z];
            v2[2] = (((z + 1) * scale) - (z * scale));
        } else {
            can_calculate = false;
        }
    }

    if (can_calculate) {
        n->compo_x = v1[1] * v2[2] - v1[2] * v2[1];
        n->compo_y = v1[2] * v2[0] - v1[0] * v2[2];
        n->compo_z = v1[0] * v2[1] - v1[1] * v2[0];
        float mag =
                sqrt((n->compo_x * n->compo_x) + (n->compo_y * n->compo_y) +
                     (n->compo_z * n->compo_z));
        n->compo_x /= mag;
        n->compo_y /= mag;
        n->compo_z /= mag;
    } else {
        n->compo_x = 0;
        n->compo_y = 1;
        n->compo_z = 0;
    }
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
    terrainGen(new_steps, new_increase, new_radius, new_random_jump);
    for (int i = -1; i < smoothness; i++) terrainSmoothe(10);

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
            Vertex v_i(j * prep_scale, th[i][j] /*SCALE*/, i * prep_scale);
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
                smoothShadeNormal(j, i, &n_i);
            }
            normals[index_normals++] = n_i;

            /************************************************************/
            /*	V_J -- N_J												*/
            /************************************************************/
            Vertex v_j(j * prep_scale,
                       th[i + 1][j] /*SCALE*/,
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
                smoothShadeNormal(j, i + 1, &n_j);
            }
            normals[index_normals++] = n_j;

            /************************************************************/
            /*	V_K -- N_K												*/
            /************************************************************/
            Vertex v_k((j + 1) * prep_scale,
                       th[i][j + 1] /*SCALE*/,
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
                smoothShadeNormal(j + 1, i, &n_k);
            }
            normals[index_normals++] = n_k;

            /************************************************************/
            /*	V_X -- N_X	(SAME AS V_J/N_J)							*/
            /************************************************************/
            Vertex v_x(j * prep_scale,
                       th[i + 1][j] /*SCALE*/,
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
                smoothShadeNormal(j, i + 1, &n_x);
            }
            normals[index_normals++] = n_x;

            /************************************************************/
            /*	V_Y -- N_Y												*/
            /************************************************************/
            Vertex v_y((j + 1) * prep_scale,
                       th[i + 1][j + 1] /*SCALE*/,
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
                smoothShadeNormal(j + 1, i + 1, &n_y);
            }
            normals[index_normals++] = n_y;

            /************************************************************/
            /*	V_Z -- N_Z												*/
            /************************************************************/
            Vertex v_z((j + 1) * prep_scale,
                       th[i][j + 1] /*SCALE*/,
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
                smoothShadeNormal(j + 1, i, &n_z);
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
                    glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                            "glGenBuffersARB")));
            pgl_bind_buffer_arb = reinterpret_cast<PFNGLBINDBUFFERARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                            "glBindBufferARB")));
            pgl_buffer_data_arb = reinterpret_cast<PFNGLBUFFERDATAARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                            "glBufferDataARB")));
            pgl_buffer_sub_data_arb =
                    reinterpret_cast<PFNGLBUFFERSUBDATAARBPROC>(
                            glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                                    "glBufferSubDataARB")));
            pgl_delete_buffers_arb =
                    reinterpret_cast<PFNGLDELETEBUFFERSARBPROC>(
                            glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                                    "glDeleteBuffersARB")));
            pgl_get_buffer_parameteriv_arb =
                    reinterpret_cast<PFNGLGETBUFFERPARAMETERIVARBPROC>(
                            glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                                    "glGetBufferParameterivARB")));
            pgl_map_buffer_arb = reinterpret_cast<PFNGLMAPBUFFERARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                            "glMapBufferARB")));
            pgl_unmap_buffer_arb = reinterpret_cast<PFNGLUNMAPBUFFERARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const GLubyte*>(
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

void TerrainMaker::prepTerrain() {
    th = new int*[size];
    for (int i = 0; i < size; i++) {
        th[i] = new int[size];
    }
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            th[x][y] = 0;
        }
    }
}

void TerrainMaker::terrainGen(int new_steps,
                              int new_increase,
                              float new_radius,
                              int new_random_jump) {
    float current_x = size / 2;
    float current_y = size / 2;
    float distance = 0;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            th[i][j] = 0;
        }
    }

    for (int current_step = 1; current_step < new_steps; current_step++) {
        int random = (rand() % 100);

        if (random > new_random_jump) {
            switch ((rand() % 4)) {
                case 0:
                    current_x--;
                    break;
                case 1:
                    current_x++;
                    break;
                case 2:
                    current_y--;
                    break;
                case 3:
                    current_y++;
                    break;
            }
            if (((current_x >= size) || (current_x < 0)) ||
                ((current_y >= size) || (current_y < 0))) {
                current_x = (rand() % size);
                current_y = (rand() % size);
            }
        } else {
            current_x = (rand() % size);
            current_y = (rand() % size);
        }

        for (int x = current_x - new_radius; x < current_x + new_radius; x++)
            for (int y = current_y - new_radius; y < current_y + new_radius;
                 y++) {
                distance = static_cast<float>(
                        sqrt(pow(static_cast<double>(current_x - x), 2) +
                             pow(static_cast<double>(current_y) - y, 2)));
                if ((distance < new_radius) &&
                    ((x >= 0 && x < size) && (y >= 0 && y < size)))
                    th[x][y] += new_increase;
            }
    }
}

void TerrainMaker::terrainSqDi(
        int left, int right, int top, int bottom, int seed, int subtract) {
    if ((left - right) == 0 || (bottom - top) == 1) {
    }  // DO NOTHING BUT RETURN
    else {
        while (seed <= 1) {
            seed = rand();
            if (seed < 0) {
                seed *= -1;
            }
        }

        th[top][left] = rand() % seed;
        th[bottom][left] = rand() % seed;
        th[top][right] = rand() % seed;
        th[bottom][right] = rand() % seed;
        th[(top + bottom) / 2][(left + right) / 2] =
                (((th[top][left]) + (th[bottom][left]) + (th[top][right]) +
                  (th[bottom][right])) /
                 4) +
                rand() % seed;

        th[(top + bottom) / 2][left] = rand() % seed;
        th[bottom][(left + right) / 2] = rand() % seed;
        th[top][(left + right) / 2] = rand() % seed;
        th[(top + bottom) / 2][right] = rand() % seed;
        th[(top + bottom) / 2][(left + right) / 2] =
                (((th[(top + bottom) / 2][left]) +
                  (th[bottom][(left + right) / 2]) +
                  (th[top][(left + right) / 2] = rand() % seed) +
                  (th[(top + bottom) / 2][right])) /
                 4) +
                rand() % seed;

        seed -= subtract;
        terrainSqDi(left,
                    (left + right) / 2,
                    top,
                    (top + bottom) / 2,
                    seed,
                    subtract);
        terrainSqDi(left,
                    (left + right) / 2,
                    (top + bottom) / 2,
                    bottom,
                    seed,
                    subtract);
        terrainSqDi((left + right) / 2,
                    right,
                    top,
                    (top + bottom) / 2,
                    seed,
                    subtract);
        terrainSqDi((left + right) / 2,
                    right,
                    (top + bottom) / 2,
                    bottom,
                    seed,
                    subtract);
    }
}

void TerrainMaker::terrainSmoothe(int box_width) {
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int height_sum = 0;
            for (int i = y - (box_width / 2); i < y + (box_width / 2); i++) {
                for (int j = x - (box_width / 2); j < x + (box_width / 2);
                     j++) {
                    if ((i >= 0 && i < size) && (j >= 0 && j < size)) {
                        height_sum += th[i][j];
                    }
                }
            }
            th[y][x] = height_sum / (box_width * box_width);
        }
    }

    //*
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (x == 0 || y == 0) {
                th[y][x] = 0;
            }
            if (x == 0 || y == size - 1) {
                th[y][x] = 0;
            }
            if (x == size - 1 || y == 0) {
                th[y][x] = 0;
            }
            if (x == size - 1 || y == size - 1) {
                th[y][x] = 0;
            }
        }
    }
    //*/
}

void TerrainMaker::terrainSlope(int new_vertices) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i < new_vertices) {
                th[i][j] -= (new_vertices - i) * scale;
            } else if (i > size - new_vertices) {
                th[i][j] -= (new_vertices - (size - i)) * scale;
            }
            if (j < new_vertices) {
                th[i][j] -= (new_vertices - j) * scale;

            } else if (j > size - new_vertices) {
                th[i][j] -= (new_vertices - (size - j)) * scale;
            }
        }
    }
}

void TerrainMaker::toggleWireframe() { wireframe_active = !wireframe_active; }

Normal TerrainMaker::getTriangleNormal(float x, float z) {
    /*	NORMAL ORIENTATION VECTOR CODE FOR TANK ORIENTATION	*/
    Vertex* v0 = new Vertex(0, 0, 0);
    Vertex* v1 = new Vertex(0, 0, 0);
    Vertex* v2 = new Vertex(0, 0, 0);
    Vertex* vert_collection[3] = {v0, v1, v2};
    collectVerticesForTriangleNormal(x, z, vert_collection);

    GLfloat u[3] = {(v1->coord_x - v0->coord_x),
                    (v1->coord_y / 100 - v0->coord_y / 100),
                    (v1->coord_z - v0->coord_z)};
    GLfloat v[3] = {(v2->coord_x - v0->coord_x),
                    (v2->coord_y / 100 - v0->coord_y / 100),
                    (v2->coord_z - v0->coord_z)};

    Normal n((u[1] * v[2] - v[1] * u[2]),
             (u[2] * v[0] - u[0] * v[2]),
             (u[0] * v[1] - v[0] * u[1]));

    GLfloat mag = static_cast<GLfloat>(
            sqrt(pow(static_cast<double>(n.compo_x), 2.0) +
                 pow(static_cast<double>(n.compo_y), 2.0) +
                 pow(static_cast<double>(n.compo_z), 2.0)));
    n.compo_x /= mag;
    n.compo_y /= mag;
    n.compo_z /= mag;
    if (n.compo_y < 0) {
        n.compo_x *= -1;
        n.compo_y *= -1;
        n.compo_z *= -1;
    }
    delete vert_collection[0];
    delete vert_collection[1];
    delete vert_collection[2];
    return Normal(n.compo_x, n.compo_y, n.compo_z);
}

Normal TerrainMaker::getNormalAt(GLfloat x, GLfloat z) {
    int n_x = static_cast<int>(x / scale);
    int n_z = static_cast<int>(z / scale);
    float d_x = x / scale - n_x;
    float d_z = z / scale - n_z;

    if (d_x > 0) {
        if (abs(d_x) > 0.5) {
            n_x++;
        }
    } else {
        if (abs(d_x) < 0.5) {
            n_x++;
        }
    }
    if (d_z > 0) {
        if (abs(d_z) > 0.5) {
            n_z++;
        }
    } else {
        if (abs(d_z) < 0.5) {
            n_z++;
        }
    }
    float n[3];
    float v[3] = {0,
                  static_cast<float>(th[n_z][n_x + 1] - th[n_z][n_x]),
                  static_cast<float>(scale)};
    float u[3] = {static_cast<float>(scale),
                  static_cast<float>(th[n_z + 1][n_x] - th[n_z][n_x]),
                  0};

    n[0] = (v[1] * u[2] - u[1] * v[2]);
    n[1] = (u[0] * v[2] - v[0] * u[2]);
    n[2] = (v[0] * u[1] - u[0] * v[1]);

    float mag = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
    n[0] /= mag;
    n[1] /= mag;
    n[2] /= mag;
    return Normal(n[0], n[1], n[2]);
    /*int i;
    for(i=0;i<(size-1)*(size-1)*6;i++){
        if((vertices[i].coordX==nX*scale)&&(vertices[i].coordZ==nZ*scale))
    break;
    }
    return &normals[i];*/
}

GLfloat TerrainMaker::getHeightAt(GLfloat x, GLfloat z) {
    if ((x >= 0 && x < (size - 1) * scale) &&
        (z >= 0 && z < (size - 1) * scale)) {
        int v_x = static_cast<int>(x / scale);
        int v_z = static_cast<int>(z / scale);
        float d_x = x / scale - v_x;
        float d_z = z / scale - v_z;

        if (d_x < 0) {
            if (abs(d_x) < 0.5) {
                v_x++;
            }
        } else {
            if (abs(d_x) > 0.5) {
                v_x++;
            }
        }
        if (d_z < 0) {
            if (abs(d_z) < 0.5) {
                v_z++;
            }
        } else {
            if (abs(d_z) > 0.5) {
                v_z++;
            }
        }
        return th[v_x][v_z];
    }
    return 0.0;
}

void TerrainMaker::collectVerticesForTriangleNormal(
        int x, int z, Vertex* three_vertices_array[3]) {
    if ((x >= 0 && x < size) && (z >= 0 && z < size)) {
        three_vertices_array[0]->coord_x = x;
        three_vertices_array[0]->coord_y = th[z][x];
        three_vertices_array[0]->coord_z = z;

        three_vertices_array[1]->coord_x = x + 1;
        three_vertices_array[1]->coord_y = th[z][x + 1];
        three_vertices_array[1]->coord_z = z;

        three_vertices_array[2]->coord_x = x;
        three_vertices_array[2]->coord_y = th[z + 1][x];
        three_vertices_array[2]->coord_z = z + 1;
    } else {
        cout << "Tank out of bounds" << endl;
    }
}

void TerrainMaker::makeCrater(GLfloat impact_x,
                              GLfloat impact_z,
                              GLfloat blast_size) {
    int x = static_cast<int>(impact_x / scale);
    int z = static_cast<int>(impact_z / scale);
    Vertex* buffer_ptr = static_cast<Vertex*>(
            pgl_map_buffer_arb(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE));
    int crater_size = static_cast<int>(blast_size * 1.5);

    if (((x >= 0) && (x < size)) && ((z >= 0) && (z < size))) {
        GLfloat impact_y = th[z][x];
        for (int i = x - crater_size; i < x + crater_size; i++) {
            for (int j = z - crater_size; j < z + crater_size; j++) {
                GLfloat distance = sqrt(static_cast<float>((x - i) * (x - i) +
                                                           (z - j) * (z - j)));
                if ((((i >= 0) && (j >= 0)) && ((i < size) && (j < size))) &&
                    (distance <= blast_size)) {
                    GLfloat x_dis, y_dis, z_dis, dist_radius;
                    x_dis = abs(i - x);
                    z_dis = abs(j - z);
                    dist_radius = sqrt(x_dis * x_dis + z_dis * z_dis);

                    GLfloat damage_depth =
                            -((sqrt(dist_radius * dist_radius + x_dis * x_dis +
                                    z_dis * z_dis) -
                               blast_size * 2) *
                              scale / 2);
                    GLfloat adjust_height = th[j][i];
                    if (adjust_height > (impact_y + (damage_depth))) {
                        adjust_height -= damage_depth;
                    } else if (adjust_height > (impact_y - damage_depth)) {
                        adjust_height = impact_y - damage_depth;
                    }
                    // lower vertex height in local th[][]
                    // lower vertex height for the 6 vertices that share the
                    // same location in VBO
                    th[j][i] = adjust_height;  // local
                    if (i == x && j == z && j > 1 && i > 1) {
                        adjust_height =
                                (th[j - 1][i] + th[j][i - 1] + th[j][i]) / 3;
                    }
                    th[j][i] = adjust_height;  // local
                    buffer_ptr[(j * (size - 1) + i) * 6].coord_y =
                            adjust_height;  // VBO
                    if ((j - 1) > 0) {
                        buffer_ptr[((j - 1) * (size - 1) + i) * 6 + 1]
                                .coord_y = adjust_height;  // VBO
                        buffer_ptr[((j - 1) * (size - 1) + i) * 6 + 3]
                                .coord_y = adjust_height;  // VBO
                        if ((i - 1) > 0) {
                            buffer_ptr[((j - 1) * (size - 1) + (i - 1)) * 6 +
                                       4]
                                    .coord_y = adjust_height;  // VBO
                        }
                    }
                    if ((i - 1) > 0) {
                        buffer_ptr[((j) * (size - 1) + (i - 1)) * 6 + 2]
                                .coord_y = adjust_height;  // VBO
                        buffer_ptr[((j) * (size - 1) + (i - 1)) * 6 + 5]
                                .coord_y = adjust_height;  // VBO
                    }
                }
            }
        }

        // adjust normals

        int normal_offset = tri_strip_buffer_size;
        for (int i = x - crater_size; i < x + crater_size; i++) {
            for (int j = z - crater_size; j < z + crater_size; j++) {
                Normal adjust_normal;
                GLfloat distance = sqrt(static_cast<float>((x - i) * (x - i) +
                                                           (z - j) * (z - j)));
                if ((i >= 0 && j >= 0 && i < size && j < size) &&
                    (distance <= blast_size)) {
                    smoothShadeNormal(j, i, &adjust_normal);
                    buffer_ptr[normal_offset + (j * (size - 1) + i) * 6]
                            .coord_x = adjust_normal.compo_x;  // VBO
                    buffer_ptr[normal_offset + (j * (size - 1) + i) * 6]
                            .coord_y = adjust_normal.compo_y;  // VBO
                    buffer_ptr[normal_offset + (j * (size - 1) + i) * 6]
                            .coord_z = adjust_normal.compo_z;  // VBO
                    if ((j - 1) > 0) {
                        buffer_ptr[normal_offset +
                                   ((j - 1) * (size - 1) + i) * 6 + 1]
                                .coord_x = adjust_normal.compo_x;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j - 1) * (size - 1) + i) * 6 + 1]
                                .coord_y = adjust_normal.compo_y;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j - 1) * (size - 1) + i) * 6 + 1]
                                .coord_z = adjust_normal.compo_z;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j - 1) * (size - 1) + i) * 6 + 3]
                                .coord_x = adjust_normal.compo_x;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j - 1) * (size - 1) + i) * 6 + 3]
                                .coord_y = adjust_normal.compo_y;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j - 1) * (size - 1) + i) * 6 + 3]
                                .coord_z = adjust_normal.compo_z;  // VBO
                        if ((i - 1) > 0) {
                            buffer_ptr[normal_offset +
                                       ((j - 1) * (size - 1) + (i - 1)) * 6 +
                                       4]
                                    .coord_x = adjust_normal.compo_x;  // VBO
                            buffer_ptr[normal_offset +
                                       ((j - 1) * (size - 1) + (i - 1)) * 6 +
                                       4]
                                    .coord_y = adjust_normal.compo_y;  // VBO
                            buffer_ptr[normal_offset +
                                       ((j - 1) * (size - 1) + (i - 1)) * 6 +
                                       4]
                                    .coord_z = adjust_normal.compo_z;  // VBO
                        }
                    }
                    if ((i - 1) > 0) {
                        buffer_ptr[normal_offset +
                                   ((j) * (size - 1) + (i - 1)) * 6 + 2]
                                .coord_x = adjust_normal.compo_x;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j) * (size - 1) + (i - 1)) * 6 + 2]
                                .coord_y = adjust_normal.compo_y;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j) * (size - 1) + (i - 1)) * 6 + 2]
                                .coord_z = adjust_normal.compo_z;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j) * (size - 1) + (i - 1)) * 6 + 5]
                                .coord_x = adjust_normal.compo_x;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j) * (size - 1) + (i - 1)) * 6 + 5]
                                .coord_y = adjust_normal.compo_y;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j) * (size - 1) + (i - 1)) * 6 + 5]
                                .coord_z = adjust_normal.compo_z;  // VBO
                    }
                }
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

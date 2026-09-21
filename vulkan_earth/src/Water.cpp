#include "Water.h"
#include <GL/glx.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include "Normal.h"
#include "Shader.h"
#include "TexCoord.h"
#include "Vertex.h"
#include "math.h"
// #include "macro_crtdbg.h"

using namespace std;

Water::Water() = default;

Water::Water(int new_scale, int new_size) {
    srand(time(nullptr));
    vbo_qualify = nullptr;
    verifyVBOs();
    timer = 0.0;
    scale = new_scale;
    size = new_size;
    total_vertices = size * size;
    tri_strip_buffer_size = (size - 1) * (size - 1) * 6;
    initData();
    prepTerrain();
    shader = new Shader();
    shader->init("VertexWater.vs", "FragmentWater.vs");
    color_texture = loadTexture("Water.raw", 1024, 1024);
    normal_texture = loadTexture("bumpMap.raw", 256, 256);
    prepareData();
}

Water::~Water() {
    if (surfaceheight != nullptr) {
        for (int i = 0; i < size; i++) {
            delete surfaceheight[i];
        }
        delete surfaceheight;
    }
    delete vbo_qualify;
    delete shader;
    pgl_delete_buffers_arb(1, &vertex_vbo_id);
    pgl_delete_buffers_arb(1, &normal_vbo_id);
    pgl_delete_buffers_arb(1, &texture_vbo_id);
    glDeleteTextures(1, &color_texture);
    glDeleteTextures(1, &normal_texture);
}

GLint Water::getScale() { return scale; }
GLint Water::getActualSize() { return (size) * (scale); }

void Water::initData() {
    vertices.resize(tri_strip_buffer_size);
    normals.resize(tri_strip_buffer_size);
    tex_coord.resize(tri_strip_buffer_size);
    material_specular = {0.0, 0.0, 0.0, 0.0};
    material_shininess = {10000.0};
    material_diffuse = {0.0, 1.0, 0.0, 1.0};
}

GLuint Water::loadTexture(const char* filename, int width, int height) {
    GLuint texture;
    std::vector<unsigned char> data(width * height * 3);
    std::ifstream file(filename, std::ios::binary);
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

void Water::draw() {
    int draw_size = size;
    int draw_scale = scale;
    int buffersize = tri_strip_buffer_size;

    shader->bind();
    glEnable(GL_LIGHTING);  // NOT PART OF SHADER CODE

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    int texture_location = glGetUniformLocation(shader->id(), "color_texture");
    glUniform1i(texture_location, 0);
    glBindTexture(GL_TEXTURE_2D, color_texture);

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    int normal_location = glGetUniformLocation(shader->id(), "normal_texture");
    glUniform1i(normal_location, 1);
    glBindTexture(GL_TEXTURE_2D, normal_texture);

    timer_loc = glGetUniformLocation(shader->id(), "timer");
    glUniform1f(timer_loc, timer);
    timer += 0.002 * 3.14159265;
    if (timer >= 2 * 3.14159265) timer = 0.0;

    glEnableClientState(GL_NORMAL_ARRAY);         // Enable Normal Arrays
    glEnableClientState(GL_VERTEX_ARRAY);         // Enable Vertex Arrays
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);  // Enable Texture Arrays
    pgl_bind_buffer_arb(GL_ARRAY_BUFFER_ARB, vertex_vbo_id);
    glVertexPointer(3, GL_FLOAT, 0, nullptr);
    glNormalPointer(
            GL_FLOAT, 0, reinterpret_cast<void*>(buffersize * sizeof(Vertex)));
    glTexCoordPointer(2,
                      GL_FLOAT,
                      0,
                      reinterpret_cast<void*>(
                              buffersize * (sizeof(Vertex) + sizeof(Normal))));
    glDrawArrays(GL_TRIANGLES, 0, buffersize);
    glDisableClientState(GL_VERTEX_ARRAY);         // Disable Vertex Arrays
    glDisableClientState(GL_NORMAL_ARRAY);         // Disable Vertex Arrays
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);  // Disable Texture Arrays

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_LIGHTING);  // NOT PART OF SHADER CODE
    shader->unbind();
}

void Water::calcAverageofSixNormals(Vertex* v_0,
                                    GLfloat x1,
                                    GLfloat y1,
                                    GLfloat z1,
                                    GLfloat x2,
                                    GLfloat y2,
                                    GLfloat z2,
                                    GLfloat x3,
                                    GLfloat y3,
                                    GLfloat z3,
                                    GLfloat x4,
                                    GLfloat y4,
                                    GLfloat z4,
                                    GLfloat x5,
                                    GLfloat y5,
                                    GLfloat z5,
                                    GLfloat x6,
                                    GLfloat y6,
                                    GLfloat z6,
                                    Normal* n) {
    GLfloat u_1_x = x1 - v_0->coord_x;
    GLfloat u_1_y = y1 - v_0->coord_y;
    GLfloat u_1_z = z1 - v_0->coord_z;
    GLfloat u_2_x = x2 - v_0->coord_x;
    GLfloat u_2_y = y2 - v_0->coord_y;
    GLfloat u_2_z = z2 - v_0->coord_z;
    GLfloat u_3_x = x3 - v_0->coord_x;
    GLfloat u_3_y = y3 - v_0->coord_y;
    GLfloat u_3_z = z3 - v_0->coord_z;
    GLfloat u_4_x = x4 - v_0->coord_x;
    GLfloat u_4_y = y4 - v_0->coord_y;
    GLfloat u_4_z = z4 - v_0->coord_z;
    GLfloat u_5_x = x5 - v_0->coord_x;
    GLfloat u_5_y = y5 - v_0->coord_y;
    GLfloat u_5_z = z5 - v_0->coord_z;
    GLfloat u_6_x = x6 - v_0->coord_x;
    GLfloat u_6_y = y6 - v_0->coord_y;
    GLfloat u_6_z = z6 - v_0->coord_z;
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
    GLfloat magnitude =
            sqrt((n->compo_x * n->compo_x) + (n->compo_y * n->compo_y) +
                 (n->compo_z * n->compo_z));
    n->compo_x /= magnitude;
    n->compo_y /= magnitude;
    n->compo_z /= magnitude;
}

void Water::verifyVBOs() {
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
                cout << "Pointers to Buffer Functions Failed to be Obtained"
                     << endl;
                exit(0);
            }
        } else {
            cout << "GL_ARB_vertex_buffer_object IS NOT Supported" << endl;
            exit(0);
        }
    } else {
        cout << "ERROR: VBO Creation Failed" << endl;
        exit(0);
    }
}

void Water::prepTerrain() {
    surfaceheight = new int*[size];
    for (int i = 0; i < size; i++) {
        surfaceheight[i] = new int[size];
    }
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            surfaceheight[x][y] = -5 * 100;
        }
    }
}

void Water::prepareData() {
    int buffersize = tri_strip_buffer_size;
    int prep_size = size;
    int prep_scale = scale;

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
    int index = 0;
    int index_normals = 0;
    int index_texture = 0;
    for (int i = 0; i < prep_size - 1; i++) {
        for (int j = 0; j < prep_size - 1; j++) {
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
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        &n_i);
            } else if (i == 0) {
                calcAverageofSixNormals(&v_i,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        static_cast<GLfloat>(j - 1),
                                        surfaceheight[i][j - 1],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j - 1),
                                        surfaceheight[i + 1][j - 1],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        &n_i);
            } else if (j == 0) {
                calcAverageofSixNormals(&v_i,
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i - 1][j + 1],
                                        static_cast<GLfloat>(i - 1),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i - 1][j],
                                        static_cast<GLfloat>(i - 1),
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        &n_i);
            } else {
                calcAverageofSixNormals(&v_i,
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i - 1][j + 1],
                                        static_cast<GLfloat>(i - 1),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i - 1][j],
                                        static_cast<GLfloat>(i - 1),
                                        static_cast<GLfloat>(j - 1),
                                        surfaceheight[i][j - 1],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j - 1),
                                        surfaceheight[i + 1][j - 1],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
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
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        v_j.coord_x,
                                        v_j.coord_y,
                                        v_j.coord_z,
                                        v_j.coord_x,
                                        v_j.coord_y,
                                        v_j.coord_z,
                                        v_j.coord_x,
                                        v_j.coord_y,
                                        v_j.coord_z,
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
                                        &n_j);
            } else if (j == 0) {
                calcAverageofSixNormals(&v_j,
                                        static_cast<GLfloat>(j) + 1,
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        v_j.coord_x,
                                        v_j.coord_y,
                                        v_j.coord_z,
                                        v_j.coord_x,
                                        v_j.coord_y,
                                        v_j.coord_z,
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 2][j],
                                        static_cast<GLfloat>(i + 2),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
                                        &n_j);
            } else if (i == prep_size - 2) {
                calcAverageofSixNormals(&v_j,
                                        static_cast<GLfloat>(j) + 1,
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j - 1),
                                        surfaceheight[i + 1][j - 1],
                                        static_cast<GLfloat>(i + 1),
                                        v_j.coord_x,
                                        v_j.coord_y,
                                        v_j.coord_z,
                                        v_j.coord_x,
                                        v_j.coord_y,
                                        v_j.coord_z,
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
                                        &n_j);
            } else {
                calcAverageofSixNormals(&v_j,
                                        static_cast<GLfloat>(j) + 1,
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j - 1),
                                        surfaceheight[i + 1][j - 1],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j - 1),
                                        surfaceheight[i + 2][j - 1],
                                        static_cast<GLfloat>(i + 2),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 2][j],
                                        static_cast<GLfloat>(i + 2),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
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
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
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
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 2),
                                        surfaceheight[i][j + 2],
                                        static_cast<GLfloat>(i),
                                        &n_k);
            } else if (j == prep_size - 2) {
                calcAverageofSixNormals(&v_k,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i - 1][j + 1],
                                        static_cast<GLfloat>(i - 1),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        &n_k);
            } else {
                calcAverageofSixNormals(&v_k,
                                        static_cast<GLfloat>(j + 2),
                                        surfaceheight[i - 1][j + 2],
                                        static_cast<GLfloat>(i - 1),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i - 1][j + 1],
                                        static_cast<GLfloat>(i - 1),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 2),
                                        surfaceheight[i][j + 2],
                                        static_cast<GLfloat>(i),
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
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        v_x.coord_x,
                                        v_x.coord_y,
                                        v_x.coord_z,
                                        v_x.coord_x,
                                        v_x.coord_y,
                                        v_x.coord_z,
                                        v_x.coord_x,
                                        v_x.coord_y,
                                        v_x.coord_z,
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
                                        &n_x);
            } else if (j == 0) {
                calcAverageofSixNormals(&v_x,
                                        static_cast<GLfloat>(j) + 1,
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        v_x.coord_x,
                                        v_x.coord_y,
                                        v_x.coord_z,
                                        v_x.coord_x,
                                        v_x.coord_y,
                                        v_x.coord_z,
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 2][j],
                                        static_cast<GLfloat>(i + 2),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
                                        &n_x);
            } else if (i == prep_size - 2) {
                calcAverageofSixNormals(&v_x,
                                        static_cast<GLfloat>(j) + 1,
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j - 1),
                                        surfaceheight[i + 1][j - 1],
                                        static_cast<GLfloat>(i + 1),
                                        v_x.coord_x,
                                        v_x.coord_y,
                                        v_x.coord_z,
                                        v_x.coord_x,
                                        v_x.coord_y,
                                        v_x.coord_z,
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
                                        &n_x);
            } else {
                calcAverageofSixNormals(&v_x,
                                        static_cast<GLfloat>(j) + 1,
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j - 1),
                                        surfaceheight[i + 1][j - 1],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j - 1),
                                        surfaceheight[i + 2][j - 1],
                                        static_cast<GLfloat>(i + 2),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 2][j],
                                        static_cast<GLfloat>(i + 2),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
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
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
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
                                        static_cast<GLfloat>(j + 2),
                                        surfaceheight[i][j + 2],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        v_y.coord_x,
                                        v_y.coord_y,
                                        v_y.coord_z,
                                        v_y.coord_x,
                                        v_y.coord_y,
                                        v_y.coord_z,
                                        static_cast<GLfloat>(j + 2),
                                        surfaceheight[i + 1][j + 2],
                                        static_cast<GLfloat>(i + 1),
                                        &n_y);
            } else if (j == prep_size - 2) {
                calcAverageofSixNormals(&v_y,
                                        v_y.coord_x,
                                        v_y.coord_y,
                                        v_y.coord_z,
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 2][j],
                                        static_cast<GLfloat>(i + 2),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 2][j + 1],
                                        static_cast<GLfloat>(i + 2),
                                        v_y.coord_x,
                                        v_y.coord_y,
                                        v_y.coord_z,
                                        &n_y);
            } else {
                calcAverageofSixNormals(&v_y,
                                        static_cast<GLfloat>(j + 2),
                                        surfaceheight[i][j + 2],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i][j + 1],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 2][j],
                                        static_cast<GLfloat>(i + 2),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 2][j + 1],
                                        static_cast<GLfloat>(i + 2),
                                        static_cast<GLfloat>(j + 2),
                                        surfaceheight[i + 1][j + 2],
                                        static_cast<GLfloat>(i + 1),
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
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
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
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 2),
                                        surfaceheight[i][j + 2],
                                        static_cast<GLfloat>(i),
                                        &n_z);
            } else if (j == prep_size - 2) {
                calcAverageofSixNormals(&v_z,
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i - 1][j + 1],
                                        static_cast<GLfloat>(i - 1),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
                                        v_i.coord_x,
                                        v_i.coord_y,
                                        v_i.coord_z,
                                        &n_z);
            } else {
                calcAverageofSixNormals(&v_z,
                                        static_cast<GLfloat>(j + 2),
                                        surfaceheight[i - 1][j + 2],
                                        static_cast<GLfloat>(i - 1),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i - 1][j + 1],
                                        static_cast<GLfloat>(i - 1),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i][j],
                                        static_cast<GLfloat>(i),
                                        static_cast<GLfloat>(j),
                                        surfaceheight[i + 1][j],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 1),
                                        surfaceheight[i + 1][j + 1],
                                        static_cast<GLfloat>(i + 1),
                                        static_cast<GLfloat>(j + 2),
                                        surfaceheight[i][j + 2],
                                        static_cast<GLfloat>(i),
                                        &n_z);
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

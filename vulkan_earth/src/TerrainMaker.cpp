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

TerrainMaker::TerrainMaker(int iScale, int iSize) {
    srand(time(nullptr));
    this->scale = iScale;
    this->size = iSize;
    totalVertices = this->size * this->size;
    triStripBufferSize = (this->size - 1) * (this->size - 1) * 6;
    prepTerrain();
    initData();
    PFNGLGENBUFFERSARBPROC pgl_gen_buffers_arb =
            nullptr;  // VBO Name Generation Procedure
    PFNGLBINDBUFFERARBPROC pgl_bind_buffer_arb =
            nullptr;  // VBO Bind Procedure
    PFNGLBUFFERDATAARBPROC pgl_buffer_data_arb =
            nullptr;  // VBO Data Loading Procedure
    PFNGLBUFFERSUBDATAARBPROC pgl_buffer_sub_data_arb =
            nullptr;  // VBO Sub Data Loading Procedure
    PFNGLDELETEBUFFERSARBPROC pgl_delete_buffers_arb =
            nullptr;  // VBO Deletion Procedure
    PFNGLGETBUFFERPARAMETERIVARBPROC pgl_get_buffer_parameteriv_arb =
            nullptr;  // return various parameters of VBO
    PFNGLMAPBUFFERARBPROC pgl_map_buffer_arb = nullptr;  // map VBO procedure
    PFNGLUNMAPBUFFERARBPROC pgl_unmap_buffer_arb =
            nullptr;  // unmap VBO procedure

    shader = new Shader();
    shader->init("VertexShader.vs", "FragmentShader.vs");
    color_texture = LoadTexture("Rocky.raw", 2048, 2048);
    normal_texture = LoadTexture("bumpMap.raw", 256, 256);

    rotation_angle = 0.0;
    vboQualify = nullptr;
    verifyVBOs();
    wireframeActive = false;
}

TerrainMaker::~TerrainMaker() {
    if (th) {
        for (int i = 0; i < size; i++) {
            delete th[i];
        }
        delete th;
    }
    delete vboQualify;
    delete shader;
    pglDeleteBuffersARB(1, &vertexVBOId);
    pglDeleteBuffersARB(1, &normalVBOId);
    pglDeleteBuffersARB(1, &textureVBOId);
    glDeleteTextures(1, &color_texture);
    glDeleteTextures(1, &normal_texture);
}

GLint TerrainMaker::getScale() { return this->scale; }
GLint TerrainMaker::getActualSize() { return (this->size) * (this->scale); }

GLuint TerrainMaker::selectTexture(const std::string& tex) {
    glDeleteTextures(1, &color_texture);

    if (tex == "Rock")
        return LoadTexture("Rocky.raw", 2048, 2048);
    else if (tex == "Snow")
        return LoadTexture("Snowy.raw", 2048, 2048);
    else if (tex == "Ice")
        return LoadTexture("Icy.raw", 2048, 2048);
    else if (tex == "Mars")
        return LoadTexture("RedPlanet.raw", 2048, 2048);
    else if (tex == "Desert")
        return LoadTexture("Desert.raw", 2048, 2048);
    else if (tex == "Lava")
        return LoadTexture("LavaRock.raw", 2048, 2048);
    else
        return 0;
}

GLuint TerrainMaker::LoadTexture(const char* filename, int width, int height) {
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
    int size = this->size;
    int scale = this->scale;
    int buffersize = triStripBufferSize;
    glEnable(GL_COLOR_MATERIAL);

    if (wireframeActive) {
        glColor4f(0, 0, 0, .75);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnableClientState(GL_VERTEX_ARRAY);
        pglBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexVBOId);
        glVertexPointer(3, GL_FLOAT, 0, nullptr);
        glDrawArrays(GL_TRIANGLES, 0, buffersize);
        glDisableClientState(GL_VERTEX_ARRAY);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glColor4f(1, 1, 1, .75);
        for (int i = 0; i < 255; i += 4) {
            for (int j = 0; j < 255; j += 4) {
                glBegin(GL_LINES);
                /*glVertex3f(i*scale,th[j][i],j*scale);
                glVertex3f(	i*scale+500*normals[(i*255+j)*6].compoX,
                            th[j][i]+500*normals[(i*255+j)*6].compoY,
                            j*scale+500*normals[(i*255+j)*6].compoZ);*/
                glVertex3f(i * scale, th[j][i], j * scale);
                glVertex3f(
                        i * scale +
                                500 * getNormalAt(i * scale, j * scale).compoX,
                        th[j][i] +
                                500 * getNormalAt(i * scale, j * scale).compoY,
                        j * scale + 500 * getNormalAt(i * scale, j * scale)
                                                    .compoZ);
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
        pglBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexVBOId);
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
    this->vertices.resize(triStripBufferSize);
    normals.resize(triStripBufferSize);
    tex_coord.resize(triStripBufferSize);
    materialSpecular = {1.0, 1.0, 1.0, 1.0};
    materialShininess = {10000.0};
    materialDiffuse = {0.0, 1.0, 0.0, 1.0};
}

void TerrainMaker::smoothShadeNormal(int x, int z, Normal* n) {
    calcNormal(x, z - 1, 0, n);
    Normal n0(n->compoX, n->compoY, n->compoZ);
    calcNormal(x, z, 0, n);
    Normal n1(n->compoX, n->compoY, n->compoZ);
    calcNormal(x, z, 1, n);
    Normal n2(n->compoX, n->compoY, n->compoZ);
    calcNormal(x + 1, z + 1, 1, n);
    Normal n3(n->compoX, n->compoY, n->compoZ);
    calcNormal(x + 1, z, 1, n);
    Normal n4(n->compoX, n->compoY, n->compoZ);
    calcNormal(x + 1, z, 0, n);
    Normal n5(n->compoX, n->compoY, n->compoZ);

    n->compoX = (n0.compoX + n1.compoX + n2.compoX + n3.compoX + n4.compoX +
                 n5.compoX) /
                6;
    n->compoY = (n0.compoY + n1.compoY + n2.compoY + n3.compoY + n4.compoY +
                 n5.compoY) /
                6;
    n->compoZ = (n0.compoZ + n1.compoZ + n2.compoZ + n3.compoZ + n4.compoZ +
                 n5.compoZ) /
                6;
}

void TerrainMaker::calcNormal(int x, int z, int flag, Normal* n) {
    float v1[3], v2[3];
    bool can_calculate = true;
    if (flag == 1) {
        if (((x - 1) >= 0) && ((z - 1) >= 0) && (x < this->size) &&
            (z < this->size)) {
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
        if ((((x - 1) >= 0) && (x < this->size)) && ((z + 1) < this->size) &&
            (z > 0)) {
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
        n->compoX = v1[1] * v2[2] - v1[2] * v2[1];
        n->compoY = v1[2] * v2[0] - v1[0] * v2[2];
        n->compoZ = v1[0] * v2[1] - v1[1] * v2[0];
        float mag = sqrt((n->compoX * n->compoX) + (n->compoY * n->compoY) +
                         (n->compoZ * n->compoZ));
        n->compoX /= mag;
        n->compoY /= mag;
        n->compoZ /= mag;
    } else {
        n->compoX = 0;
        n->compoY = 1;
        n->compoZ = 0;
    }
}

void TerrainMaker::prepareData(int steps,
                               int increase,
                               float radius,
                               int randomJump,
                               int smoothness) {
    int chunk_size = this->size / 2;
    this->steps = steps;
    this->increase = increase;
    this->radius = radius;
    this->randomJump = randomJump;
    int buffersize = triStripBufferSize;
    int size = this->size;
    int scale = this->scale;
    terrainGen(steps, increase, radius, randomJump);
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
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - 1; j++) {
            /************************************************************/
            /*	V_I -- N_I												*/
            /************************************************************/
            Vertex v_i(j * scale, th[i][j] /*SCALE*/, i * scale);
            this->vertices[index++] = v_i;
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
            Vertex v_j(j * scale, th[i + 1][j] /*SCALE*/, (i + 1) * scale);
            this->vertices[index++] = v_j;
            TexCoord t_j((static_cast<float>(i % (chunk_size - 1)) + 1) /
                                 static_cast<float>(chunk_size - 1),
                         (static_cast<float>(j % (chunk_size - 1))) /
                                 static_cast<float>(chunk_size - 1));
            tex_coord[index_texture++] = t_j;
            Normal n_j(0, 0, 0);
            if (i == size - 2 && j == 0) {
            } else if (j == 0) {
            } else if (i == size - 2) {
            } else {
                smoothShadeNormal(j, i + 1, &n_j);
            }
            normals[index_normals++] = n_j;

            /************************************************************/
            /*	V_K -- N_K												*/
            /************************************************************/
            Vertex v_k((j + 1) * scale, th[i][j + 1] /*SCALE*/, (i)*scale);
            this->vertices[index++] = v_k;
            TexCoord t_k((static_cast<float>(i % (chunk_size - 1))) /
                                 static_cast<float>(chunk_size - 1),
                         (static_cast<float>(j % (chunk_size - 1)) + 1) /
                                 static_cast<float>(chunk_size - 1));
            tex_coord[index_texture++] = t_k;
            Normal n_k(0, 0, 0);
            if (i == 0 && j == size - 2) {
            } else if (i == 0) {
            } else if (j == size - 2) {
            } else {
                smoothShadeNormal(j + 1, i, &n_k);
            }
            normals[index_normals++] = n_k;

            /************************************************************/
            /*	V_X -- N_X	(SAME AS V_J/N_J)							*/
            /************************************************************/
            Vertex v_x(j * scale, th[i + 1][j] /*SCALE*/, (i + 1) * scale);
            this->vertices[index++] = v_x;
            TexCoord t_x((static_cast<float>(i % (chunk_size - 1)) + 1) /
                                 static_cast<float>(chunk_size - 1),
                         (static_cast<float>(j % (chunk_size - 1))) /
                                 static_cast<float>(chunk_size - 1));
            tex_coord[index_texture++] = t_x;
            Normal n_x(0, 0, 0);
            if (i == size - 2 && j == 0) {
            } else if (j == 0) {
            } else if (i == size - 2) {
            } else {
                smoothShadeNormal(j, i + 1, &n_x);
            }
            normals[index_normals++] = n_x;

            /************************************************************/
            /*	V_Y -- N_Y												*/
            /************************************************************/
            Vertex v_y((j + 1) * scale,
                       th[i + 1][j + 1] /*SCALE*/,
                       (i + 1) * scale);
            this->vertices[index++] = v_y;
            TexCoord t_y((static_cast<float>(i % (chunk_size - 1)) + 1) /
                                 static_cast<float>(chunk_size - 1),
                         (static_cast<float>(j % (chunk_size - 1)) + 1) /
                                 static_cast<float>(chunk_size - 1));
            tex_coord[index_texture++] = t_y;
            Normal n_y(0, 0, 0);
            if (i == size - 2 && j == size - 2) {
            } else if (i == size - 2) {
            } else if (j == size - 2) {
            } else {
                smoothShadeNormal(j + 1, i + 1, &n_y);
            }
            normals[index_normals++] = n_y;

            /************************************************************/
            /*	V_Z -- N_Z												*/
            /************************************************************/
            Vertex v_z((j + 1) * scale, th[i][j + 1] /*SCALE*/, (i)*scale);
            this->vertices[index++] = v_z;
            TexCoord t_z((static_cast<float>(i % (chunk_size - 1))) /
                                 static_cast<float>(chunk_size - 1),
                         (static_cast<float>(j % (chunk_size - 1)) + 1) /
                                 static_cast<float>(chunk_size - 1));
            tex_coord[index_texture++] = t_z;
            Normal n_z(0, 0, 0);
            if (i == 0 && j == size - 2) {
            } else if (i == 0) {
            } else if (j == size - 2) {
            } else {
                smoothShadeNormal(j + 1, i, &n_z);
            }
            normals[index_normals++] = n_z;
        }
    }

    pglGenBuffersARB(1, &vertexVBOId);  // Create VBO for Vertices
    pglBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexVBOId);
    pglBufferDataARB(
            GL_ARRAY_BUFFER_ARB,
            buffersize * (sizeof(Vertex) + sizeof(Normal) + sizeof(TexCoord)),
            nullptr,
            GL_DYNAMIC_DRAW_ARB);

    pglBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
                        0,
                        buffersize * sizeof(Vertex),
                        this->vertices.data());

    pglBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
                        buffersize * sizeof(Vertex),
                        buffersize * sizeof(Normal),
                        normals.data());

    pglBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
                        buffersize * (sizeof(Vertex) + sizeof(Normal)),
                        buffersize * sizeof(TexCoord),
                        tex_coord.data());
}

void TerrainMaker::verifyVBOs() {
    delete vboQualify;
    vboQualify = new VBOQualifer();
    vboQualify->establishIfQualified();
    if (vboQualify->getQualified()) {
        if (vboQualify->isExtensionSupported("GL_ARB_vertex_buffer_object")) {
            pglGenBuffersARB = reinterpret_cast<PFNGLGENBUFFERSARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                            "glGenBuffersARB")));
            pglBindBufferARB = reinterpret_cast<PFNGLBINDBUFFERARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                            "glBindBufferARB")));
            pglBufferDataARB = reinterpret_cast<PFNGLBUFFERDATAARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                            "glBufferDataARB")));
            pglBufferSubDataARB = reinterpret_cast<PFNGLBUFFERSUBDATAARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                            "glBufferSubDataARB")));
            pglDeleteBuffersARB = reinterpret_cast<PFNGLDELETEBUFFERSARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                            "glDeleteBuffersARB")));
            pglGetBufferParameterivARB =
                    reinterpret_cast<PFNGLGETBUFFERPARAMETERIVARBPROC>(
                            glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                                    "glGetBufferParameterivARB")));
            pglMapBufferARB = reinterpret_cast<PFNGLMAPBUFFERARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                            "glMapBufferARB")));
            pglUnmapBufferARB = reinterpret_cast<PFNGLUNMAPBUFFERARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                            "glUnmapBufferARB")));
            if (pglGenBuffersARB && pglBindBufferARB && pglBufferDataARB &&
                pglBufferSubDataARB && pglDeleteBuffersARB &&
                pglGetBufferParameterivARB && pglMapBufferARB &&
                pglUnmapBufferARB) {
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
    th = new int*[this->size];
    for (int i = 0; i < this->size; i++) {
        th[i] = new int[this->size];
    }
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            th[x][y] = 0;
        }
    }
}

void TerrainMaker::terrainGen(int steps,
                              int increase,
                              float radius,
                              int randomJump) {
    float current_x = this->size / 2;
    float current_y = this->size / 2;
    float distance = 0;

    for (int i = 0; i < this->size; i++) {
        for (int j = 0; j < this->size; j++) {
            th[i][j] = 0;
        }
    }

    for (int current_step = 1; current_step < steps; current_step++) {
        int random = (rand() % 100);

        if (random > randomJump) {
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
            if (((current_x >= this->size) || (current_x < 0)) ||
                ((current_y >= this->size) || (current_y < 0))) {
                current_x = (rand() % this->size);
                current_y = (rand() % this->size);
            }
        } else {
            current_x = (rand() % this->size);
            current_y = (rand() % this->size);
        }

        for (int x = current_x - radius; x < current_x + radius; x++)
            for (int y = current_y - radius; y < current_y + radius; y++) {
                distance = static_cast<float>(
                        sqrt(pow(static_cast<double>(current_x - x), 2) +
                             pow(static_cast<double>(current_y) - y, 2)));
                if ((distance < radius) &&
                    ((x >= 0 && x < this->size) && (y >= 0 && y < this->size)))
                    th[x][y] += increase;
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
    for (int y = 0; y < this->size; y++) {
        for (int x = 0; x < this->size; x++) {
            int sum = 0;
            for (int i = y - (box_width / 2); i < y + (box_width / 2); i++) {
                for (int j = x - (box_width / 2); j < x + (box_width / 2);
                     j++) {
                    if ((i >= 0 && i < this->size) &&
                        (j >= 0 && j < this->size)) {
                        sum += th[i][j];
                    }
                }
            }
            th[y][x] = sum / (box_width * box_width);
        }
    }

    //*
    for (int y = 0; y < this->size; y++) {
        for (int x = 0; x < this->size; x++) {
            if (x == 0 || y == 0) {
                th[y][x] = 0;
            }
            if (x == 0 || y == this->size - 1) {
                th[y][x] = 0;
            }
            if (x == this->size - 1 || y == 0) {
                th[y][x] = 0;
            }
            if (x == this->size - 1 || y == this->size - 1) {
                th[y][x] = 0;
            }
        }
    }
    //*/
}

void TerrainMaker::terrainSlope(int vertices) {
    for (int i = 0; i < this->size; i++) {
        for (int j = 0; j < this->size; j++) {
            if (i < vertices) {
                th[i][j] -= (vertices - i) * this->scale;
            } else if (i > this->size - vertices) {
                th[i][j] -= (vertices - (this->size - i)) * this->scale;
            }
            if (j < vertices) {
                th[i][j] -= (vertices - j) * this->scale;

            } else if (j > this->size - vertices) {
                th[i][j] -= (vertices - (this->size - j)) * this->scale;
            }
        }
    }
}

void TerrainMaker::toggleWireframe() { wireframeActive = !wireframeActive; }

Normal TerrainMaker::getTriangleNormal(float x, float z) {
    /*	NORMAL ORIENTATION VECTOR CODE FOR TANK ORIENTATION	*/
    Vertex* v0 = new Vertex(0, 0, 0);
    Vertex* v1 = new Vertex(0, 0, 0);
    Vertex* v2 = new Vertex(0, 0, 0);
    Vertex* vert_collection[3] = {v0, v1, v2};
    collectVerticesForTriangleNormal(x, z, vert_collection);

    GLfloat u[3] = {(v1->coordX - v0->coordX),
                    (v1->coordY / 100 - v0->coordY / 100),
                    (v1->coordZ - v0->coordZ)};
    GLfloat v[3] = {(v2->coordX - v0->coordX),
                    (v2->coordY / 100 - v0->coordY / 100),
                    (v2->coordZ - v0->coordZ)};

    Normal n((u[1] * v[2] - v[1] * u[2]),
             (u[2] * v[0] - u[0] * v[2]),
             (u[0] * v[1] - v[0] * u[1]));

    GLfloat mag = static_cast<GLfloat>(
            sqrt(pow(static_cast<double>(n.compoX), 2.0) +
                 pow(static_cast<double>(n.compoY), 2.0) +
                 pow(static_cast<double>(n.compoZ), 2.0)));
    n.compoX /= mag;
    n.compoY /= mag;
    n.compoZ /= mag;
    if (n.compoY < 0) {
        n.compoX *= -1;
        n.compoY *= -1;
        n.compoZ *= -1;
    }
    delete vert_collection[0];
    delete vert_collection[1];
    delete vert_collection[2];
    return Normal(n.compoX, n.compoY, n.compoZ);
}

Normal TerrainMaker::getNormalAt(GLfloat x, GLfloat z) {
    int n_x = static_cast<int>(x / this->scale);
    int n_z = static_cast<int>(z / this->scale);
    float d_x = x / this->scale - n_x;
    float d_z = z / this->scale - n_z;

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
                  static_cast<float>(this->scale)};
    float u[3] = {static_cast<float>(this->scale),
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
    for(i=0;i<(this->size-1)*(this->size-1)*6;i++){
        if((this->vertices[i].coordX==nX*this->scale)&&(this->vertices[i].coordZ==nZ*this->scale))
    break;
    }
    return &normals[i];*/
}

GLfloat TerrainMaker::getHeightAt(GLfloat x, GLfloat z) {
    if ((x >= 0 && x < (this->size - 1) * this->scale) &&
        (z >= 0 && z < (this->size - 1) * this->scale)) {
        int v_x = static_cast<int>(x / this->scale);
        int v_z = static_cast<int>(z / this->scale);
        float d_x = x / this->scale - v_x;
        float d_z = z / this->scale - v_z;

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
    if ((x >= 0 && x < this->size) && (z >= 0 && z < this->size)) {
        three_vertices_array[0]->coordX = x;
        three_vertices_array[0]->coordY = th[z][x];
        three_vertices_array[0]->coordZ = z;

        three_vertices_array[1]->coordX = x + 1;
        three_vertices_array[1]->coordY = th[z][x + 1];
        three_vertices_array[1]->coordZ = z;

        three_vertices_array[2]->coordX = x;
        three_vertices_array[2]->coordY = th[z + 1][x];
        three_vertices_array[2]->coordZ = z + 1;
    } else {
        cout << "Tank out of bounds" << endl;
    }
}

void TerrainMaker::makeCrater(GLfloat fx, GLfloat fz, GLfloat blastSize) {
    int x = static_cast<int>(fx / this->scale);
    int z = static_cast<int>(fz / this->scale);
    Vertex* buffer_ptr = static_cast<Vertex*>(
            pglMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE));
    int crater_size = static_cast<int>(blastSize * 1.5);

    if (((x >= 0) && (x < size)) && ((z >= 0) && (z < size))) {
        GLfloat impact_y = th[z][x];
        for (int i = x - crater_size; i < x + crater_size; i++) {
            for (int j = z - crater_size; j < z + crater_size; j++) {
                GLfloat distance = sqrt(static_cast<float>((x - i) * (x - i) +
                                                           (z - j) * (z - j)));
                if ((((i >= 0) && (j >= 0)) &&
                     ((i < this->size) && (j < this->size))) &&
                    (distance <= blastSize)) {
                    GLfloat x_dis, y_dis, z_dis, radius;
                    x_dis = abs(i - x);
                    z_dis = abs(j - z);
                    radius = sqrt(x_dis * x_dis + z_dis * z_dis);

                    GLfloat damage_depth =
                            -((sqrt(radius * radius + x_dis * x_dis +
                                    z_dis * z_dis) -
                               blastSize * 2) *
                              this->scale / 2);
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
                    buffer_ptr[(j * (this->size - 1) + i) * 6].coordY =
                            adjust_height;  // VBO
                    if ((j - 1) > 0) {
                        buffer_ptr[((j - 1) * (this->size - 1) + i) * 6 + 1]
                                .coordY = adjust_height;  // VBO
                        buffer_ptr[((j - 1) * (this->size - 1) + i) * 6 + 3]
                                .coordY = adjust_height;  // VBO
                        if ((i - 1) > 0) {
                            buffer_ptr[((j - 1) * (this->size - 1) + (i - 1)) *
                                               6 +
                                       4]
                                    .coordY = adjust_height;  // VBO
                        }
                    }
                    if ((i - 1) > 0) {
                        buffer_ptr[((j) * (this->size - 1) + (i - 1)) * 6 + 2]
                                .coordY = adjust_height;  // VBO
                        buffer_ptr[((j) * (this->size - 1) + (i - 1)) * 6 + 5]
                                .coordY = adjust_height;  // VBO
                    }
                }
            }
        }

        // adjust normals

        int normal_offset = triStripBufferSize;
        for (int i = x - crater_size; i < x + crater_size; i++) {
            for (int j = z - crater_size; j < z + crater_size; j++) {
                Normal adjust_normal;
                GLfloat distance = sqrt(static_cast<float>((x - i) * (x - i) +
                                                           (z - j) * (z - j)));
                if ((i >= 0 && j >= 0 && i < this->size && j < this->size) &&
                    (distance <= blastSize)) {
                    smoothShadeNormal(j, i, &adjust_normal);
                    buffer_ptr[normal_offset + (j * (this->size - 1) + i) * 6]
                            .coordX = adjust_normal.compoX;  // VBO
                    buffer_ptr[normal_offset + (j * (this->size - 1) + i) * 6]
                            .coordY = adjust_normal.compoY;  // VBO
                    buffer_ptr[normal_offset + (j * (this->size - 1) + i) * 6]
                            .coordZ = adjust_normal.compoZ;  // VBO
                    if ((j - 1) > 0) {
                        buffer_ptr[normal_offset +
                                   ((j - 1) * (this->size - 1) + i) * 6 + 1]
                                .coordX = adjust_normal.compoX;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j - 1) * (this->size - 1) + i) * 6 + 1]
                                .coordY = adjust_normal.compoY;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j - 1) * (this->size - 1) + i) * 6 + 1]
                                .coordZ = adjust_normal.compoZ;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j - 1) * (this->size - 1) + i) * 6 + 3]
                                .coordX = adjust_normal.compoX;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j - 1) * (this->size - 1) + i) * 6 + 3]
                                .coordY = adjust_normal.compoY;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j - 1) * (this->size - 1) + i) * 6 + 3]
                                .coordZ = adjust_normal.compoZ;  // VBO
                        if ((i - 1) > 0) {
                            buffer_ptr[normal_offset +
                                       ((j - 1) * (this->size - 1) + (i - 1)) *
                                               6 +
                                       4]
                                    .coordX = adjust_normal.compoX;  // VBO
                            buffer_ptr[normal_offset +
                                       ((j - 1) * (this->size - 1) + (i - 1)) *
                                               6 +
                                       4]
                                    .coordY = adjust_normal.compoY;  // VBO
                            buffer_ptr[normal_offset +
                                       ((j - 1) * (this->size - 1) + (i - 1)) *
                                               6 +
                                       4]
                                    .coordZ = adjust_normal.compoZ;  // VBO
                        }
                    }
                    if ((i - 1) > 0) {
                        buffer_ptr[normal_offset +
                                   ((j) * (this->size - 1) + (i - 1)) * 6 + 2]
                                .coordX = adjust_normal.compoX;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j) * (this->size - 1) + (i - 1)) * 6 + 2]
                                .coordY = adjust_normal.compoY;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j) * (this->size - 1) + (i - 1)) * 6 + 2]
                                .coordZ = adjust_normal.compoZ;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j) * (this->size - 1) + (i - 1)) * 6 + 5]
                                .coordX = adjust_normal.compoX;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j) * (this->size - 1) + (i - 1)) * 6 + 5]
                                .coordY = adjust_normal.compoY;  // VBO
                        buffer_ptr[normal_offset +
                                   ((j) * (this->size - 1) + (i - 1)) * 6 + 5]
                                .coordZ = adjust_normal.compoZ;  // VBO
                    }
                }
            }
        }
    }
    pglUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
}

/************************************************************************/
/*	Debugging MessageBox functions										*/
/************************************************************************/
void TerrainMaker::stdMessageBox(const std::string& output) {}

void TerrainMaker::errorMessageBox(const std::string& output) {}

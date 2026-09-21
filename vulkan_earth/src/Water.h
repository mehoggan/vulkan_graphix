#ifndef WATER_H_
#define WATER_H_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <vector>
#include "Normal.h"
#include "TexCoord.h"
#include "VBOQualifer.h"
#include "Vertex.h"
class Shader;

class Water {
public:
    Water();
    Water(int scale, int size);
    ~Water();
    void draw();
    void initData();
    void prepareData(int steps, int increase, float radius, int random_jump);
    void calcAverageofSixNormals(Vertex* v_0,
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
                                 Normal* n);
    void verifyVBOs();
    void prepTerrain();
    void prepareData();
    void terrainGen(int steps, int increase, float radius, int random_jump);
    GLint getActualSize();
    GLint getScale();
    void stdMessageBox(const std::string& output);
    void errorMessageBox(const std::string& output);
    GLuint loadTexture(const char* filename, int width, int height);

private:
    VBOQualifer* vbo_qualify;
    int scale;
    int size;
    int total_vertices;
    int tri_strip_buffer_size;
    int** surfaceheight;
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<TexCoord> tex_coord;
    GLuint color_texture;
    GLuint normal_texture;
    std::vector<GLfloat> material_specular;
    std::vector<GLfloat> material_shininess;
    std::vector<GLfloat> material_diffuse;
    void configVBOs();
    unsigned int vertex_vbo_id;
    unsigned int normal_vbo_id;
    unsigned int texture_vbo_id;
    PFNGLGENBUFFERSARBPROC
    pgl_gen_buffers_arb;  // VBO Name Generation Procedure
    PFNGLBINDBUFFERARBPROC pgl_bind_buffer_arb;  // VBO Bind Procedure
    PFNGLBUFFERDATAARBPROC pgl_buffer_data_arb;  // VBO Data Loading Procedure
    PFNGLBUFFERSUBDATAARBPROC
    pgl_buffer_sub_data_arb;  // VBO Sub Data Loading Procedure
    PFNGLDELETEBUFFERSARBPROC
    pgl_delete_buffers_arb;  // VBO Deletion Procedure
    PFNGLGETBUFFERPARAMETERIVARBPROC
    pgl_get_buffer_parameteriv_arb;  // return various parameters of VBO
    PFNGLMAPBUFFERARBPROC pgl_map_buffer_arb;      // map VBO procedure
    PFNGLUNMAPBUFFERARBPROC pgl_unmap_buffer_arb;  // unmap VBO procedure
    Shader* shader;
    GLint timer_loc;
    GLfloat timer;
};

#endif /*	WATER_H_	*/

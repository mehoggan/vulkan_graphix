#ifndef TERRAINMAKER
#define TERRAINMAKER

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "Normal.h"
#include "TexCoord.h"
#include "VBOQualifer.h"
#include "Vertex.h"
class Shader;

class TerrainMaker {
public:
    TerrainMaker();
    TerrainMaker(int i_scale, int i_size);
    ~TerrainMaker();
    void draw();
    void initData();
    void prepareData(int steps,
                     int increase,
                     float radius,
                     int random_jump,
                     int smoothness);
    void smoothShadeNormal(int x, int z, Normal* n);
    void calcNormal(int x, int z, int flag, Normal* n);
    void verifyVBOs();
    void prepTerrain();
    void terrainSlope(int vertices);
    void terrainGen(int steps, int increase, float radius, int random_jump);
    void terrainSqDi(
            int left, int right, int top, int bottom, int seed, int subtract);
    void terrainSmoothe(int box_width);
    void stdMessageBox(const std::string& output);
    void errorMessageBox(const std::string& output);
    void toggleWireframe();
    void makeCrater(GLfloat x, GLfloat z, GLfloat size);
    Normal getTriangleNormal(float x, float z);
    Normal getNormalAt(GLfloat x, GLfloat z);
    GLfloat getHeightAt(GLfloat x, GLfloat z);
    GLint getActualSize();
    GLint getScale();
    GLuint loadTexture(const char* filename, int width, int height);
    GLuint selectTexture(const std::string& tex);
    void collectVerticesForTriangleNormal(int x,
                                          int z,
                                          Vertex* three_vertices_array[3]);

private:
    int scale;
    int size;
    int steps;
    int increase;
    float radius;
    int random_jump;
    int total_vertices;
    int tri_strip_buffer_size;
    int** th;
    VBOQualifer* vbo_qualify;
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<TexCoord> tex_coord;
    GLuint color_texture;
    GLuint normal_texture;
    std::vector<GLfloat> material_specular;
    std::vector<GLfloat> material_shininess;
    std::vector<GLfloat> material_diffuse;
    void configVBOs();
    GLuint vertex_vbo_id;
    GLuint normal_vbo_id;
    GLuint texture_vbo_id;
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
    GLfloat rotation_angle;
    bool wireframe_active;
};

#endif  //	TERRAINMAKER
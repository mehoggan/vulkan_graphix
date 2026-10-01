#ifndef TERRAINMAKER
#define TERRAINMAKER

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "vulkan_earth/Normal.h"
#include "vulkan_earth/TexCoord.h"
#include "vulkan_earth/VBOQualifer.h"
#include "vulkan_earth/Vertex.h"
#include "vulkan_graphix/TerrainGenerator.h"
class Shader;

// The height field itself - generation, normals, world-position queries,
// and crater deformation - is libvulkan_graphix's TerrainGenerator (shared
// with the Vulkan tutorials); TerrainMaker keeps only its GL buffers/draw
// code plus thin wrappers converting to this game's own Normal type.

class TerrainMaker {
public:
    TerrainMaker(int i_scale, int i_size);
    ~TerrainMaker();
    void draw();
    void initData();
    void prepareData(int new_steps,
                     int new_increase,
                     float new_radius,
                     int new_random_jump,
                     int smoothness);
    void verifyVBOs();
    void stdMessageBox(const std::string& output);
    void errorMessageBox(const std::string& output);
    void toggleWireframe();
    void makeCrater(GLfloat x, GLfloat z, GLfloat size);
    // x/z in grid units (truncated to a grid vertex).
    Normal getTriangleNormal(float x, float z);
    // x/z in world units.
    Normal getNormalAt(GLfloat x, GLfloat z);
    GLfloat getHeightAt(GLfloat x, GLfloat z);
    GLint getActualSize();
    GLint getScale();
    GLuint loadTexture(const char* filename, int width, int height);
    GLuint selectTexture(const std::string& tex);

private:
    int scale;
    int size;
    int steps;
    int increase;
    float radius;
    int random_jump;
    int total_vertices;
    int tri_strip_buffer_size;
    vulkan_graphix::TerrainGenerator terrain;
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
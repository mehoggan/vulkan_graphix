#ifndef WATER_H_
#define WATER_H_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <cstdint>
#include <vector>
#include "vulkan_earth/Normal.h"
#include "vulkan_earth/TexCoord.h"
#include "vulkan_earth/VBOQualifer.h"
#include "vulkan_earth/Vertex.h"
class Shader;

class Water {
public:
    Water();
    Water(int new_scale, int new_size);
    ~Water();
    void draw();
    void initData();
    void prepareData(int steps, int increase, float radius, int random_jump);
    void calcAverageofSixNormals(Vertex* v_0,
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
                                 Normal* n);
    void verifyVBOs();
    void prepTerrain();
    void prepareData();
    void terrainGen(int steps, int increase, float radius, int random_jump);
    std::int32_t getActualSize();
    std::int32_t getScale();
    void stdMessageBox(const std::string& output);
    void errorMessageBox(const std::string& output);
    std::uint32_t loadTexture(const char* filename, int width, int height);

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
    std::uint32_t color_texture;
    std::uint32_t normal_texture;
    std::vector<float> material_specular;
    std::vector<float> material_shininess;
    std::vector<float> material_diffuse;
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
    std::int32_t timer_loc;
    float timer;
};

#endif /*	WATER_H_	*/

#ifndef TERRAINMAKER
#define TERRAINMAKER

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <cstdint>
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
    TerrainMaker(std::int32_t i_scale, std::int32_t i_size);
    ~TerrainMaker();
    void draw();
    void initData();
    void prepareData(std::int32_t new_steps,
                     std::int32_t new_increase,
                     float new_radius,
                     std::int32_t new_random_jump,
                     std::int32_t smoothness);
    void verifyVBOs();
    void stdMessageBox(const std::string& output);
    void errorMessageBox(const std::string& output);
    void toggleWireframe();
    void makeCrater(float x, float z, float size);
    // x/z in grid units (truncated to a grid vertex).
    Normal getTriangleNormal(float x, float z);
    // x/z in world units.
    Normal getNormalAt(float x, float z);
    float getHeightAt(float x, float z);
    std::int32_t getActualSize();
    std::int32_t getScale();
    std::uint32_t loadTexture(const char* filename,
                              std::int32_t width,
                              std::int32_t height);
    std::uint32_t selectTexture(const std::string& tex);

private:
    std::int32_t scale;
    std::int32_t size;
    std::int32_t steps;
    std::int32_t increase;
    float radius;
    std::int32_t random_jump;
    std::int32_t total_vertices;
    std::int32_t tri_strip_buffer_size;
    vulkan_graphix::TerrainGenerator terrain;
    VBOQualifer* vbo_qualify;
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<TexCoord> tex_coord;
    std::uint32_t color_texture;
    std::uint32_t normal_texture;
    std::vector<float> material_specular;
    std::vector<float> material_shininess;
    std::vector<float> material_diffuse;
    void configVBOs();
    std::uint32_t vertex_vbo_id;
    std::uint32_t normal_vbo_id;
    std::uint32_t texture_vbo_id;
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
    float rotation_angle;
    bool wireframe_active;
};

#endif  //	TERRAINMAKER
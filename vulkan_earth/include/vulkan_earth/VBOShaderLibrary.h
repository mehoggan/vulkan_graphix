#ifndef VBO_SHADER_LIBRARY_H_
#define VBO_SHADER_LIBRARY_H_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>
#include <string>
#include <vector>
#include "vulkan_earth/Normal.h"
#include "vulkan_earth/TexCoord.h"
#include "vulkan_earth/Vertex.h"

class VBOShaderLibrary {
public:
    VBOShaderLibrary();
    ~VBOShaderLibrary();
    void drawClientData();
    bool loadShaders(const char* vs_file_name, const char* fs_file_name);
    bool loadClientData(const std::string& model_file);
    bool loadClientData(float* vertex_data,
                        float* normal_data,
                        float* tex_coord_data,
                        std::int32_t number_of_vertices);
    void swapTexture(const char* filename,
                     std::int32_t width,
                     std::int32_t height);
    void loadTexture(const char* filename,
                     std::int32_t width,
                     std::int32_t height);
    void swapTextureNormals(const char* filename,
                            std::int32_t width,
                            std::int32_t height);
    void loadTextureNormals(const char* filename,
                            std::int32_t width,
                            std::int32_t height);
    bool getVBOPointerFunctions();
    static bool initGlew();
    static bool areVbOsSupported();

private:
    bool use_vb_os;
    bool use_textures;
    bool use_shaders;
    std::int32_t vertices_loaded;
    std::uint32_t vbo_id;
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<TexCoord> tex_coord;
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

    std::string vs_text;
    std::string fs_text;
    std::uint32_t shader_id;
    std::uint32_t shader_vp;
    std::uint32_t shader_fp;
    std::uint32_t color_texture;
    std::uint32_t normal_texture;
};

#endif /* VBO_SHADER_LIBRARY_H_	*/

#ifndef VBO_SHADER_LIBRARY_H_
#define VBO_SHADER_LIBRARY_H_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <vector>
#include "Normal.h"
#include "TexCoord.h"
#include "Vertex.h"

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
                        int number_of_vertices);
    void swapTexture(const char* filename, int width, int height);
    void loadTexture(const char* filename, int width, int height);
    void swapTextureNormals(const char* filename, int width, int height);
    void loadTextureNormals(const char* filename, int width, int height);
    bool getVBOPointerFunctions();
    static bool initGlew();
    static bool areVbOsSupported();

private:
    bool use_vb_os;
    bool use_textures;
    bool use_shaders;
    int vertices_loaded;
    GLuint vbo_id;
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
    unsigned int shader_id;
    unsigned int shader_vp;
    unsigned int shader_fp;
    GLuint color_texture;
    GLuint normal_texture;
};

#endif /* VBO_SHADER_LIBRARY_H_	*/

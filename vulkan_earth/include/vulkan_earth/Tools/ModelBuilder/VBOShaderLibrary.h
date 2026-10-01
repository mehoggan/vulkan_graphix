#ifndef VBO_SHADER_LIBRARY_H_
#define VBO_SHADER_LIBRARY_H_

#include <glew.h>
#include <glut.h>
#include <cstdint>

class Normal;
class TexCoord;
class Vertex;

class VBOShaderLibrary {
public:
    VBOShaderLibrary();
    ~VBOShaderLibrary();
    void drawClientData();
    bool loadShaders(const char* vsFileName, const char* fsFileName);
    bool loadClientData(char* modelFile);
    bool loadClientData(float* vertices,
                        float* normals,
                        float* tex_coord,
                        std::int32_t number_of_vertices);
    void SwapTexture(const char* filename,
                     std::int32_t width,
                     std::int32_t height);
    void LoadTexture(const char* filename,
                     std::int32_t width,
                     std::int32_t height);
    void SwapTextureNormals(const char* filename,
                            std::int32_t width,
                            std::int32_t height);
    void LoadTextureNormals(const char* filename,
                            std::int32_t width,
                            std::int32_t height);
    bool getVBOPointerFunctions();
    static bool InitGlew();
    static bool AreVBOsSupported();

private:
    bool useVBOs;
    bool useTextures;
    bool useShaders;
    std::int32_t verticesLoaded;
    std::uint32_t VBOId;
    Vertex* vertices;
    Normal* normals;
    TexCoord* tex_coord;
    PFNGLGENBUFFERSARBPROC pglGenBuffersARB;  // VBO Name Generation Procedure
    PFNGLBINDBUFFERARBPROC pglBindBufferARB;  // VBO Bind Procedure
    PFNGLBUFFERDATAARBPROC pglBufferDataARB;  // VBO Data Loading Procedure
    PFNGLBUFFERSUBDATAARBPROC
    pglBufferSubDataARB;  // VBO Sub Data Loading Procedure
    PFNGLDELETEBUFFERSARBPROC pglDeleteBuffersARB;  // VBO Deletion Procedure
    PFNGLGETBUFFERPARAMETERIVARBPROC
    pglGetBufferParameterivARB;             // return various parameters of VBO
    PFNGLMAPBUFFERARBPROC pglMapBufferARB;  // map VBO procedure
    PFNGLUNMAPBUFFERARBPROC pglUnmapBufferARB;  // unmap VBO procedure

    char* vsText;
    char* fsText;
    std::uint32_t shader_id;
    std::uint32_t shader_vp;
    std::uint32_t shader_fp;
    std::uint32_t color_texture;
    std::uint32_t normal_texture;
};

#endif /* VBO_SHADER_LIBRARY_H_	*/

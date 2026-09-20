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
    TerrainMaker(int iScale, int iSize);
    ~TerrainMaker();
    void draw();
    void initData();
    void prepareData(int steps,
                     int increase,
                     float radius,
                     int randomJump,
                     int smoothness);
    void smoothShadeNormal(int x, int z, Normal* n);
    void calcNormal(int x, int z, int flag, Normal* n);
    void verifyVBOs();
    void prepTerrain();
    void terrainSlope(int vertices);
    void terrainGen(int steps, int increase, float radius, int randomJump);
    void terrainSqDi(
            int left, int right, int top, int bottom, int seed, int subtract);
    void terrainSmoothe(int box_width);
    void stdMessageBox(const std::string& output);
    void errorMessageBox(const std::string& output);
    void toggleWireframe();
    void makeCrater(GLfloat x, GLfloat z, GLfloat size);
    Normal getTriangleNormal(float X, float Z);
    Normal getNormalAt(GLfloat x, GLfloat z);
    GLfloat getHeightAt(GLfloat x, GLfloat z);
    GLint getActualSize();
    GLint getScale();
    GLuint LoadTexture(const char* filename, int width, int height);
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
    int randomJump;
    int totalVertices;
    int triStripBufferSize;
    int** th;
    VBOQualifer* vboQualify;
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<TexCoord> tex_coord;
    GLuint color_texture;
    GLuint normal_texture;
    std::vector<GLfloat> materialSpecular;
    std::vector<GLfloat> materialShininess;
    std::vector<GLfloat> materialDiffuse;
    void configVBOs();
    GLuint vertexVBOId;
    GLuint normalVBOId;
    GLuint textureVBOId;
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
    Shader* shader;
    GLfloat rotation_angle;
    bool wireframeActive;
};

#endif  //	TERRAINMAKER
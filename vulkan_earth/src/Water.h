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
    void prepareData(int steps, int increase, float radius, int randomJump);
    void calcAverageofSixNormals(Vertex* v_0,
                                 GLfloat x1,
                                 GLfloat y1,
                                 GLfloat Z1,
                                 GLfloat x2,
                                 GLfloat y2,
                                 GLfloat Z2,
                                 GLfloat x3,
                                 GLfloat y3,
                                 GLfloat Z3,
                                 GLfloat x4,
                                 GLfloat y4,
                                 GLfloat Z4,
                                 GLfloat x5,
                                 GLfloat y5,
                                 GLfloat Z5,
                                 GLfloat x6,
                                 GLfloat y6,
                                 GLfloat z6,
                                 Normal* n);
    void verifyVBOs();
    void prepTerrain();
    void prepareData();
    void terrainGen(int steps, int increase, float radius, int randomJump);
    GLint getActualSize();
    GLint getScale();
    void stdMessageBox(const std::string& output);
    void errorMessageBox(const std::string& output);
    GLuint LoadTexture(const char* filename, int width, int height);

private:
    VBOQualifer* vboQualify;
    int scale;
    int size;
    int totalVertices;
    int triStripBufferSize;
    int** surfaceheight;
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<TexCoord> tex_coord;
    GLuint color_texture;
    GLuint normal_texture;
    std::vector<GLfloat> materialSpecular;
    std::vector<GLfloat> materialShininess;
    std::vector<GLfloat> materialDiffuse;
    void configVBOs();
    unsigned int vertexVBOId;
    unsigned int normalVBOId;
    unsigned int textureVBOId;
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
    GLint timerLoc;
    GLfloat timer;
};

#endif /*	WATER_H_	*/

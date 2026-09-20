#ifndef TERRAINMAKER
#define TERRAINMAKER

#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "VBOQualifer.h"
class Vertex;
class Normal;
class TexCoord;
class Shader;

class TerrainMaker {
public:
	TerrainMaker();
	TerrainMaker(int iScale, int iSize);
	~TerrainMaker();
	void draw();
	void initData();
	void prepareData(int steps, int increase, float radius, int randomJump,int smoothness);
	void smoothShadeNormal(int x,int z, Normal* n);
	void calcNormal(int x,int z,int flag , Normal* n);
	void verifyVBOs();
	void prepTerrain();
	void terrainSlope(int vertices);
	void terrainGen(int steps, int increase, float radius, int randomJump);
	void terrainSqDi(int left, int right, int top, int bottom, int seed, int subtract);
	void terrainSmoothe(int box_width);
	void stdMessageBox(char* output);
	void errorMessageBox(char* output);
	void toggleWireframe();
	void makeCrater(GLfloat x, GLfloat z, GLfloat size);
	Normal getTriangleNormal(float X, float Z);
	Normal getNormalAt(GLfloat x, GLfloat z);
	GLfloat getHeightAt(GLfloat x, GLfloat z);
	GLint getActualSize();
	GLint getScale();
	GLuint LoadTexture(const char * filename, int width, int height);
	GLuint selectTexture(char* tex);
	void collectVerticesForTriangleNormal(int x, int z, Vertex* three_vertices_array[3]);
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
	Vertex* vertices;
	Normal* normals;
	TexCoord* tex_coord;
	GLuint color_texture;
	GLuint normal_texture;
	FILE* file;
	unsigned char* data;
	GLfloat* materialSpecular;
	GLfloat* materialShininess;
	GLfloat* materialDiffuse;
	FILE* heightMapFile;
	unsigned char* heightDataContainer;
	void configVBOs();
	GLuint vertexVBOId;
	GLuint normalVBOId;
	GLuint textureVBOId;
	PFNGLGENBUFFERSARBPROC pglGenBuffersARB;                     // VBO Name Generation Procedure
	PFNGLBINDBUFFERARBPROC pglBindBufferARB;                     // VBO Bind Procedure
	PFNGLBUFFERDATAARBPROC pglBufferDataARB;                     // VBO Data Loading Procedure
	PFNGLBUFFERSUBDATAARBPROC pglBufferSubDataARB;               // VBO Sub Data Loading Procedure
	PFNGLDELETEBUFFERSARBPROC pglDeleteBuffersARB;               // VBO Deletion Procedure
	PFNGLGETBUFFERPARAMETERIVARBPROC pglGetBufferParameterivARB; // return various parameters of VBO
	PFNGLMAPBUFFERARBPROC pglMapBufferARB;                       // map VBO procedure
	PFNGLUNMAPBUFFERARBPROC pglUnmapBufferARB;                   // unmap VBO procedure
	Shader* shader;
	GLfloat rotation_angle;
	bool wireframeActive;
};

#endif	//	TERRAINMAKER
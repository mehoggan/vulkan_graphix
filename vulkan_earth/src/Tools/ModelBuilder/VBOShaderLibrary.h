#ifndef VBO_SHADER_LIBRARY_H_
#define VBO_SHADER_LIBRARY_H_

#include <glew.h>
#include <glut.h>

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
		bool loadClientData(float* vertices, float* normals, float* tex_coord, int number_of_vertices); 
		void SwapTexture(const char* filename, int width, int height);
		void LoadTexture(const char* filename, int width, int height);
		void SwapTextureNormals(const char* filename, int width, int height);
		void LoadTextureNormals(const char* filename, int width, int height);
		bool getVBOPointerFunctions();
		static bool InitGlew();
		static bool AreVBOsSupported();
	private:
		bool useVBOs;
		bool useTextures;
		bool useShaders;
		int verticesLoaded;
		GLuint VBOId;
		Vertex* vertices;
		Normal* normals;
		TexCoord* tex_coord;
		PFNGLGENBUFFERSARBPROC pglGenBuffersARB;                     // VBO Name Generation Procedure
		PFNGLBINDBUFFERARBPROC pglBindBufferARB;                     // VBO Bind Procedure
		PFNGLBUFFERDATAARBPROC pglBufferDataARB;                     // VBO Data Loading Procedure
		PFNGLBUFFERSUBDATAARBPROC pglBufferSubDataARB;               // VBO Sub Data Loading Procedure
		PFNGLDELETEBUFFERSARBPROC pglDeleteBuffersARB;               // VBO Deletion Procedure
		PFNGLGETBUFFERPARAMETERIVARBPROC pglGetBufferParameterivARB; // return various parameters of VBO
		PFNGLMAPBUFFERARBPROC pglMapBufferARB;                       // map VBO procedure
		PFNGLUNMAPBUFFERARBPROC pglUnmapBufferARB;                   // unmap VBO procedure

		char* vsText;
		char* fsText;
		unsigned int shader_id;
		unsigned int shader_vp;
		unsigned int shader_fp;
		GLuint color_texture;
		GLuint normal_texture;
};

#endif /* VBO_SHADER_LIBRARY_H_	*/

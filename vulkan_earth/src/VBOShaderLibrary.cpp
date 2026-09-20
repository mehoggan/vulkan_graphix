#include "VBOShaderLibrary.h"
#include <GL/glx.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <istream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include "Normal.h"
#include "TexCoord.h"
#include "Vertex.h"

using namespace std;

VBOShaderLibrary::VBOShaderLibrary() {
    PFNGLGENBUFFERSARBPROC pglGenBuffersARB =
            nullptr;  // VBO Name Generation Procedure
    PFNGLBINDBUFFERARBPROC pglBindBufferARB = nullptr;  // VBO Bind Procedure
    PFNGLBUFFERDATAARBPROC pglBufferDataARB =
            nullptr;  // VBO Data Loading Procedure
    PFNGLBUFFERSUBDATAARBPROC pglBufferSubDataARB =
            nullptr;  // VBO Sub Data Loading Procedure
    PFNGLDELETEBUFFERSARBPROC pglDeleteBuffersARB =
            nullptr;  // VBO Deletion Procedure
    PFNGLGETBUFFERPARAMETERIVARBPROC pglGetBufferParameterivARB =
            nullptr;  // return various parameters of VBO
    PFNGLMAPBUFFERARBPROC pglMapBufferARB = nullptr;  // map VBO procedure
    PFNGLUNMAPBUFFERARBPROC pglUnmapBufferARB =
            nullptr;  // unmap VBO procedure
    this->useVBOs = false;
    this->useTextures = false;
    this->useShaders = false;
    this->vsText = nullptr;
    this->fsText = nullptr;
    this->color_texture = 0;
    this->normal_texture = 0;
    this->verticesLoaded = 0;
}

VBOShaderLibrary::~VBOShaderLibrary() {
    this->pglDeleteBuffersARB(1, &VBOId);
    glDetachShader(this->shader_id, this->shader_fp);
    glDetachShader(this->shader_id, this->shader_vp);
    glDeleteShader(this->shader_fp);
    glDeleteShader(this->shader_vp);
    glDeleteProgram(this->shader_id);
    glDeleteTextures(1, &color_texture);
    glDeleteTextures(1, &normal_texture);
}

void VBOShaderLibrary::drawClientData() {
    if (this->useShaders) {
        glUseProgram(this->shader_id);
    }
    if (this->useTextures) {
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (!this->useShaders) {
            glBindTexture(GL_TEXTURE_2D, this->color_texture);
        }
        if (this->useShaders) {
            glEnable(GL_TEXTURE_2D);
            glActiveTexture(GL_TEXTURE0);
            int texture_location =
                    glGetUniformLocation(this->shader_id, "color_texture");
            glUniform1i(texture_location, 0);
            glBindTexture(GL_TEXTURE_2D, this->color_texture);

            glEnable(GL_TEXTURE_2D);
            glActiveTexture(GL_TEXTURE1);
            int normal_location =
                    glGetUniformLocation(this->shader_id, "normal_texture");
            glUniform1i(normal_location, 1);
            glBindTexture(GL_TEXTURE_2D, this->normal_texture);
        }
    }
    if (this->useVBOs) {
        glColor3f(1.0, 0.0, 0.0);
        int SIZE = this->verticesLoaded;
        this->pglBindBufferARB(GL_ARRAY_BUFFER_ARB, this->VBOId);

        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, (void*)nullptr);
        glNormalPointer(GL_FLOAT, 0, (void*)(SIZE * sizeof(Vertex)));
        glTexCoordPointer(2,
                          GL_FLOAT,
                          0,
                          (void*)(SIZE * (sizeof(Vertex) + sizeof(Normal))));
        glDrawArrays(GL_TRIANGLES, 0, SIZE);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    if (this->useTextures) {
        glBindTexture(GL_TEXTURE_2D, 0);
        if (this->useShaders) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
        }
        glDisable(GL_TEXTURE_2D);
    }
    if (this->useShaders) {
        glUseProgram(0);
    }
}

bool VBOShaderLibrary::loadShaders(const char* vsFileName,
                                   const char* fsFileName) {
    bool shader_status = true;
    this->useShaders = true;
    this->shader_vp = glCreateShader(GL_VERTEX_SHADER);
    this->shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

    FILE* vs_file;
    vs_file = fopen(vsFileName, "rt");
    if (vs_file) {
        fseek(vs_file, 0, SEEK_END);
        int count = ftell(vs_file);
        rewind(vs_file);
        if (count > 0) {
            this->vsText = new char[count + 1];
            count = fread(this->vsText, sizeof(char), count, vs_file);
            this->vsText[count] = '\0';
        } else {
            // shader_status = false;
            // this->useShaders = false;
        }
        fclose(vs_file);
    } else {
        // shader_status = false;
        // this->useShaders = false;
    }

    FILE* fs_file;
    fs_file = fopen(fsFileName, "rt");
    if (fs_file) {
        fseek(fs_file, 0, SEEK_END);
        int count = ftell(fs_file);
        rewind(fs_file);
        if (count > 0) {
            this->fsText = new char[count + 1];
            count = fread(this->fsText, sizeof(char), count, fs_file);
            this->fsText[count] = '\0';
        } else {
            // shader_status = false;
            // this->useShaders = false;
        }
        fclose(fs_file);
    } else {
        // shader_status = false;
        // this->useShaders = false;
    }

    /*	ERROR: NO CODE IN SHADERS	*/
    if (!this->vsText || !this->fsText) {
        cerr << "ERROR: Either vertex shader or fragment shader file not "
                "found."
             << endl;
        // shader_status = false;
        // this->useShaders = false;
    }

    /*	COMPILE AND VALIDATE THE VERTEX SHADER	*/
    const char* tempVSText = this->vsText;
    glShaderSource(this->shader_vp, 1, &tempVSText, nullptr);
    glCompileShader(this->shader_vp);
    const unsigned int VERTEX_BUFFER_SIZE = 2048;
    char buffer1[VERTEX_BUFFER_SIZE];
    memset(buffer1, 0, VERTEX_BUFFER_SIZE);
    GLsizei length1 = 0;
    glGetShaderInfoLog(this->shader_vp, VERTEX_BUFFER_SIZE, &length1, buffer1);
    if (length1 > 0) {
        cerr << "(" << vsFileName << ") -- " << buffer1 << endl;
        // shader_status = false;
        // this->useShaders = false;
    }
    memset(buffer1, 0, VERTEX_BUFFER_SIZE);

    /*	COMPILE AND VALIDATE THE FRAGMENT SHADER	*/
    const char* tempFSText = this->fsText;
    glShaderSource(this->shader_fp, 1, &tempFSText, nullptr);
    glCompileShader(this->shader_fp);
    const unsigned int FRAGMENT_BUFFER_SIZE = 2048;
    char buffer2[FRAGMENT_BUFFER_SIZE];
    memset(buffer2, 0, FRAGMENT_BUFFER_SIZE);
    GLsizei length2 = 0;
    glGetShaderInfoLog(
            this->shader_fp, FRAGMENT_BUFFER_SIZE, &length2, buffer2);
    if (length2 > 0) {
        cerr << "(" << fsFileName << ") -- " << buffer2 << endl;
        // shader_status = false;
        // this->useShaders = false;
    }
    memset(buffer2, 0, FRAGMENT_BUFFER_SIZE);

    /*	COMPILE SHADER INTO ONE AND VALIDATE PROGRAM	*/
    this->shader_id = glCreateProgram();
    glAttachShader(this->shader_id, this->shader_fp);
    glAttachShader(this->shader_id, this->shader_vp);
    glLinkProgram(this->shader_id);
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;
    glGetProgramInfoLog(this->shader_id, BUFFER_SIZE, &length, buffer);
    if (length > 0) {
        if (strcmp(buffer,
                   "Fragment shader(s) linked, vertex shader(s) linked.") !=
            0) {  // Handle Radeon Success
            cerr << "Program " << this->shader_id << " link error: " << buffer
                 << endl;
            // shader_status = false;
            // this->useShaders = false;
        }
    }
    glValidateProgram(this->shader_id);
    GLint status;
    glGetProgramiv(this->shader_id, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        cerr << "Error validating shader " << this->shader_id << endl;
        // shader_status = false;
        // this->useShaders = false;
    }
    memset(buffer, 0, BUFFER_SIZE);

    delete fsText;
    delete vsText;

    return shader_status;
}

bool VBOShaderLibrary::loadClientData(char* modelFile) {
    bool contentLoaded = true;
    unsigned int c;
    bool done = false;

    FILE* ogl_file =
            fopen(modelFile,
                  "r");  // open output file for writing and write "header"
    if (!ogl_file) {
        printf("ERROR: File %s not found\n", modelFile);
        contentLoaded = false;
    } else if (contentLoaded && ogl_file) {
        fseek(ogl_file, 0, SEEK_END);
        int read_file_size = ftell(ogl_file);
        rewind(ogl_file);
        char* buffer;
        buffer = static_cast<char*>(malloc(sizeof(char) * read_file_size + 1));
        size_t result = fread(buffer, 1, read_file_size, ogl_file);

        string str(buffer);
        stringstream strstr(str);
        istream_iterator<std::string> it(strstr);
        istream_iterator<std::string> end;
        vector<string> results(it, end);

        vector<float> vert_vec(0);
        vector<float> texcoord_vec(0);
        vector<float> normal_vec(0);

        // cout << "Total data loaded is " << results.size() << endl;
        int line_number = 0;
        if (results.size() % 8 != 0) {
            // cerr << "Wrong number of data stored results.size()%8 = " <<
            // (results.size()%8) <<  endl; exit(0);
        }
        for (int x = 0; x < results.size() - (results.size() % 8); x += 8) {
            /*	GRAB 1 TEXCOORD PAIR	*/
            float tc1 = 0.0; /*	TexCoord S	*/
            std::istringstream iss0(results[x]);
            iss0 >> tc1 >> std::dec;
            texcoord_vec.push_back(tc1);
            float tc2 = 0.0; /*	TexCoord T	*/
            std::istringstream iss1(results[x + 1]);
            iss1 >> tc2 >> std::dec;
            texcoord_vec.push_back(tc2);

            /*	GRAB 1 NORMAL TRIPLE	*/
            float n1 = 0.0; /*	Normal X	*/
            std::istringstream iss2(results[x + 2]);
            iss2 >> n1 >> std::dec;
            normal_vec.push_back(n1);
            float n2 = 0.0; /*	Normal Y	*/
            std::istringstream iss3(results[x + 3]);
            iss3 >> n2 >> std::dec;
            normal_vec.push_back(n2);
            float n3 = 0.0; /*	Normal Z	*/
            std::istringstream iss4(results[x + 4]);
            iss4 >> n3 >> std::dec;
            normal_vec.push_back(n3);

            /*	GRAB 1 VERTEX TRIPLE	*/
            float v1 = 0.0; /*	Vertex X	*/
            std::istringstream iss5(results[x + 5]);
            iss5 >> v1 >> std::dec;
            vert_vec.push_back(v1);
            float v2 = 0.0; /*	Vertex Y	*/
            std::istringstream iss6(results[x + 6]);
            iss6 >> v2 >> std::dec;
            vert_vec.push_back(v2);
            float v3 = 0.0; /*	Vertex Z	*/
            std::istringstream iss7(results[x + 7]);
            iss7 >> v3 >> std::dec;
            vert_vec.push_back(v3);

            this->verticesLoaded++;
            line_number += 3;
        }

        this->vertices = new Vertex[this->verticesLoaded];
        this->normals = new Normal[this->verticesLoaded];
        this->tex_coord = new TexCoord[this->verticesLoaded];

        int vert_index = 0;
        int vert_insert_index = 0;
        int norm_index = 0;
        int norm_insert_index = 0;
        int texc_index = 0;
        int texc_insert_index = 0;

        for (; texc_index < texcoord_vec.size();
             vert_index += 3, norm_index += 3, texc_index += 2) {
            ////cout << vert_vec[vert_index + 0] << " " << vert_vec[vert_index
            ///+ 1] << " " << vert_vec[vert_index + 2] << endl;
            Vertex v(vert_vec[vert_index + 0],
                     vert_vec[vert_index + 1],
                     vert_vec[vert_index + 2]);
            this->vertices[vert_insert_index++] = v;

            ////cout << normal_vec[norm_index + 0] << " " <<
            /// normal_vec[norm_index + 1] << " " << normal_vec[norm_index + 2]
            ///<< endl;
            Normal n(normal_vec[norm_index + 0],
                     normal_vec[norm_index + 1],
                     normal_vec[norm_index + 2]);
            this->normals[norm_insert_index++] = n;

            ////cout << texcoord_vec[texc_index + 0] << " " <<
            /// texcoord_vec[texc_index + 1] << endl;
            TexCoord t(texcoord_vec[texc_index + 0],
                       texcoord_vec[texc_index + 1]);
            this->tex_coord[texc_insert_index++] = t;
        }

        try {
            int SIZE = this->verticesLoaded;
            this->pglGenBuffersARB(1, &VBOId);
            this->pglBindBufferARB(GL_ARRAY_BUFFER_ARB, VBOId);
            this->pglBufferDataARB(GL_ARRAY_BUFFER_ARB,
                                   SIZE * (sizeof(Vertex) + sizeof(Normal) +
                                           sizeof(TexCoord)),
                                   nullptr,
                                   GL_DYNAMIC_DRAW_ARB);
            this->pglBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
                                      0,
                                      SIZE * sizeof(Vertex),
                                      this->vertices);
            this->pglBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
                                      SIZE * sizeof(Vertex),
                                      SIZE * sizeof(Normal),
                                      this->normals);
            this->pglBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
                                      SIZE * (sizeof(Vertex) + sizeof(Normal)),
                                      SIZE * sizeof(TexCoord),
                                      this->tex_coord);
            // cout << "VBOs Initialized" << endl;
            this->useVBOs = true;
        } catch (...) {
            cerr << "ERROR: When Loading VBOs" << endl;
            contentLoaded = false;
            this->useVBOs = false;
        }

        if (buffer) {
            free(buffer);
        }
        fclose(ogl_file);

        delete[] this->vertices;
        delete[] this->normals;
        delete[] this->tex_coord;
    }
    return contentLoaded;
}

bool VBOShaderLibrary::loadClientData(float* V,
                                      float* N,
                                      float* T,
                                      int number_of_vertices) {
    bool contentLoaded = true;
    try {
        this->vertices = new Vertex[number_of_vertices];
        this->normals = new Normal[number_of_vertices];
        this->tex_coord = new TexCoord[number_of_vertices];

        // cout << "Vertices Data " << endl;
        int vert_index = 0;
        for (int x = 0; x < number_of_vertices * 3; x += 3) {
            Vertex v(V[x], V[x + 1], V[x + 2]);
            this->vertices[vert_index] = v;
            // cout	<< "<" << this->vertices[vert_index].coordX << ", "
            //		<< this->vertices[vert_index].coordY << ", "
            //		<< this->vertices[vert_index].coordZ
            //		<< ">" << endl;
            vert_index++;
        }
        // cout << "Normal Data" << endl;
        int norm_index = 0;
        for (int y = 0; y < number_of_vertices * 3; y += 3) {
            Normal n(N[y], N[y + 1], N[y + 2]);
            this->normals[norm_index] = n;
            // cout	<< "<" << this->normals[norm_index].compoX << ", "
            //		<< this->normals[norm_index].compoY << ", "
            //		<< this->normals[norm_index].compoZ
            //		<< ">" << endl;
            norm_index++;
        }
        // cout << "TexCoord Data " << endl;
        int tex_coord_index = 0;
        for (int z = 0; z < number_of_vertices * 2; z += 2) {
            TexCoord t(T[z], T[z + 1]);
            this->tex_coord[tex_coord_index] = t;
            // cout	<< "<" << tex_coord[tex_coord_index].texcoordS << ", "
            //		<< this->tex_coord[tex_coord_index].texcoordT
            //		<< ">" << endl;
            tex_coord_index++;
        }
        // cout << endl;

        this->verticesLoaded = number_of_vertices;
        int SIZE = this->verticesLoaded;
        this->pglGenBuffersARB(1, &VBOId);
        this->pglBindBufferARB(GL_ARRAY_BUFFER_ARB, VBOId);
        this->pglBufferDataARB(
                GL_ARRAY_BUFFER_ARB,
                SIZE * (sizeof(Vertex) + sizeof(Normal) + sizeof(TexCoord)),
                nullptr,
                GL_DYNAMIC_DRAW_ARB);
        this->pglBufferSubDataARB(
                GL_ARRAY_BUFFER_ARB, 0, SIZE * sizeof(Vertex), this->vertices);
        this->pglBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
                                  SIZE * sizeof(Vertex),
                                  SIZE * sizeof(Normal),
                                  this->normals);
        this->pglBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
                                  SIZE * (sizeof(Vertex) + sizeof(Normal)),
                                  SIZE * sizeof(TexCoord),
                                  this->tex_coord);
        // cout << "VBOs Initialized" << endl;
        this->useVBOs = true;
    } catch (...) {
        cerr << "ERROR: When Loading VBOs" << endl;
        contentLoaded = false;
        this->useVBOs = false;
    }
    if (this->vertices && this->normals && this->tex_coord) {
        delete[] this->vertices;
        delete[] this->normals;
        delete[] this->tex_coord;
    }
    return contentLoaded;
}

void VBOShaderLibrary::SwapTexture(const char* filename,
                                   int width,
                                   int height) {
    if (!this->color_texture) {
        cerr << "ERROR: You must first load a texture" << endl;
    } else {
        glDeleteTextures(1, &this->color_texture);
        this->LoadTexture(filename, width, height);
    }
}

void VBOShaderLibrary::LoadTexture(const char* filename,
                                   int width,
                                   int height) {
    unsigned char* data;
    FILE* file;
    file = fopen(filename, "rb");
    if (!file) {
        // cout << "ERROR: File Does Not Exist Not Going to Use Textures" <<
        // endl;
    } else {
        data = new unsigned char[width * height * 3];
        fread(data, width * height * 3, 1, file);
        fclose(file);
        glGenTextures(1, &(this->color_texture));
        glBindTexture(GL_TEXTURE_2D, this->color_texture);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGB,
                     width,
                     height,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     data);
        delete[] data;
        this->useTextures = true;
        // cout << "Done Loading Image" << endl;
    }
}

void VBOShaderLibrary::SwapTextureNormals(const char* filename,
                                          int width,
                                          int height) {
    if (!this->color_texture) {
        cerr << "ERROR: You must first load a normal texture" << endl;
    } else {
        glDeleteTextures(1, &this->color_texture);
        this->LoadTexture(filename, width, height);
    }
}

void VBOShaderLibrary::LoadTextureNormals(const char* filename,
                                          int width,
                                          int height) {
    unsigned char* data;
    FILE* file;
    file = fopen(filename, "rb");
    if (!file) {
        cerr << "ERROR: File Does Not Exist Not Going to Use Texture Normals"
             << endl;
    } else {
        data = new unsigned char[width * height * 3];
        fread(data, width * height * 3, 1, file);
        fclose(file);
        glGenTextures(1, &(this->normal_texture));
        glBindTexture(GL_TEXTURE_2D, this->normal_texture);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGB,
                     width,
                     height,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     data);
        delete[] data;
        this->useTextures = true;
        // cout << "Done Loading Image" << endl;
    }
}

bool VBOShaderLibrary::getVBOPointerFunctions() {
    bool status = true;
    this->pglGenBuffersARB =
            reinterpret_cast<PFNGLGENBUFFERSARBPROC>(glXGetProcAddress(
                    reinterpret_cast<const GLubyte*>("glGenBuffersARB")));
    this->pglBindBufferARB =
            reinterpret_cast<PFNGLBINDBUFFERARBPROC>(glXGetProcAddress(
                    reinterpret_cast<const GLubyte*>("glBindBufferARB")));
    this->pglBufferDataARB =
            reinterpret_cast<PFNGLBUFFERDATAARBPROC>(glXGetProcAddress(
                    reinterpret_cast<const GLubyte*>("glBufferDataARB")));
    this->pglBufferSubDataARB =
            reinterpret_cast<PFNGLBUFFERSUBDATAARBPROC>(glXGetProcAddress(
                    reinterpret_cast<const GLubyte*>("glBufferSubDataARB")));
    this->pglDeleteBuffersARB =
            reinterpret_cast<PFNGLDELETEBUFFERSARBPROC>(glXGetProcAddress(
                    reinterpret_cast<const GLubyte*>("glDeleteBuffersARB")));
    this->pglGetBufferParameterivARB =
            reinterpret_cast<PFNGLGETBUFFERPARAMETERIVARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                            "glGetBufferParameterivARB")));
    this->pglMapBufferARB =
            reinterpret_cast<PFNGLMAPBUFFERARBPROC>(glXGetProcAddress(
                    reinterpret_cast<const GLubyte*>("glMapBufferARB")));
    this->pglUnmapBufferARB =
            reinterpret_cast<PFNGLUNMAPBUFFERARBPROC>(glXGetProcAddress(
                    reinterpret_cast<const GLubyte*>("glUnmapBufferARB")));
    if (!this->pglGenBuffersARB || !this->pglBindBufferARB ||
        !this->pglBufferDataARB || !this->pglBufferSubDataARB ||
        !this->pglDeleteBuffersARB || !this->pglGetBufferParameterivARB ||
        !this->pglMapBufferARB || !this->pglUnmapBufferARB) {
        printf("ERROR: VBO Initializtion Failed");
        status = false;
    }
    if (status) {
        // printf("VBOs Available for Object %x\n",this);
    }
    return status;
}

/*	STATIC FUNCTION */
bool VBOShaderLibrary::InitGlew() {
    bool rtn = false;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    } else {
        // fprintf(stdout, "Status: Using GLEW (%s)\n",
        // glewGetString(GLEW_VERSION));
        rtn = true;
    }
    return rtn;
}

/*	STATIC FUNCTION */
bool VBOShaderLibrary::AreVBOsSupported() {
    bool qualified = true;
    char* str = nullptr;
    char* vendor;
    char* renderer;
    char* version;
    int extensions_supported;
    char** extensions;
    int redBits;
    int greenBits;
    int blueBits;
    int alphaBits;
    int depthBits;
    int stencilBits;
    int maxTextureSize;
    int maxLights;
    int maxAttribStacks;
    int maxModelViewStacks;
    int maxProjectionStacks;
    int maxClipPlanes;
    int maxTextureStacks;

    str = (char*)glGetString(GL_VENDOR);
    if (str) {
        vendor = str;
    } else {
        qualified = false;
    }

    str = (char*)glGetString(GL_RENDERER);
    if (str) {
        renderer = str;
    } else {
        qualified = false;
    }

    str = (char*)glGetString(GL_VERSION);
    if (str) {
        version = str;
    } else {
        qualified = false;
    }

    extensions_supported = 0;
    str = (char*)glGetString(GL_EXTENSIONS);
    if (str) {
        char* tok = str;
        int index = 0;
        int size = strlen(tok);
        int iteration = 0;
        while (iteration++ < size) {
            if (*(tok + index) == ' ' || *(tok + index) == '\n' ||
                *(tok + index) == '\t') {
                extensions_supported++;
            }
            index++;
        }
        extensions = new char*[extensions_supported];
    } else {
        qualified = false;
    }

    char* tok = str;
    int size = strlen(tok);
    int array_index = 0;
    int char_pos = 0;
    int iteration = 0;
    char extension[256];
    memset(extension, 0, 256);
    while (iteration < size) {
        if (*(tok + iteration) != ' ') {
            extension[char_pos++] = *(tok + iteration);
        } else {
            extensions[array_index] = new char[sizeof(extension)];
            strcpy(extensions[array_index], extension);
            array_index++;
            char_pos = 0;
            memset(extension, 0, 256);
        }
        iteration++;
    }

    glGetIntegerv(GL_RED_BITS, &redBits);
    glGetIntegerv(GL_GREEN_BITS, &greenBits);
    glGetIntegerv(GL_BLUE_BITS, &blueBits);
    glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
    glGetIntegerv(GL_DEPTH_BITS, &depthBits);
    glGetIntegerv(GL_STENCIL_BITS, &stencilBits);
    glGetIntegerv(GL_MAX_LIGHTS, &maxLights);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    glGetIntegerv(GL_MAX_CLIP_PLANES, &maxClipPlanes);
    glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &maxModelViewStacks);
    glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &maxProjectionStacks);
    glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH, &maxAttribStacks);
    glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &maxTextureStacks);

    const char* exten = "GL_ARB_vertex_buffer_object";

    bool extension_exists = false;
    for (int x = 0; x < extensions_supported; x++) {
        const char* compare = extensions[x];
        if (strcmp(compare, exten) == 0) {
            extension_exists = true;
        }
    }
    if (!extension_exists) {
        qualified = false;
    }

    for (int i = 0; i < extensions_supported; i++) {
        if (extensions[i]) {
            delete[] extensions[i];
            extensions[i] = nullptr;
        }
    }

    delete[] extensions;

    if (qualified) {
        // fprintf(stdout, "Status: GL_ARB VBOs supported (%s)\n",
        // (char*)glGetString(GL_VERSION));
    }

    return qualified;
}
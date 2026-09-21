#include "VBOShaderLibrary.h"
#include <GL/glx.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <cstring>
#include <fstream>
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
    use_vb_os = false;
    use_textures = false;
    use_shaders = false;
    color_texture = 0;
    normal_texture = 0;
    vertices_loaded = 0;
}

VBOShaderLibrary::~VBOShaderLibrary() {
    pgl_delete_buffers_arb(1, &vbo_id);
    glDetachShader(shader_id, shader_fp);
    glDetachShader(shader_id, shader_vp);
    glDeleteShader(shader_fp);
    glDeleteShader(shader_vp);
    glDeleteProgram(shader_id);
    glDeleteTextures(1, &color_texture);
    glDeleteTextures(1, &normal_texture);
}

void VBOShaderLibrary::drawClientData() {
    if (use_shaders) {
        glUseProgram(shader_id);
    }
    if (use_textures) {
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (!use_shaders) {
            glBindTexture(GL_TEXTURE_2D, color_texture);
        }
        if (use_shaders) {
            glEnable(GL_TEXTURE_2D);
            glActiveTexture(GL_TEXTURE0);
            int texture_location =
                    glGetUniformLocation(shader_id, "color_texture");
            glUniform1i(texture_location, 0);
            glBindTexture(GL_TEXTURE_2D, color_texture);

            glEnable(GL_TEXTURE_2D);
            glActiveTexture(GL_TEXTURE1);
            int normal_location =
                    glGetUniformLocation(shader_id, "normal_texture");
            glUniform1i(normal_location, 1);
            glBindTexture(GL_TEXTURE_2D, normal_texture);
        }
    }
    if (use_vb_os) {
        glColor3f(1.0, 0.0, 0.0);
        int size = vertices_loaded;
        pgl_bind_buffer_arb(GL_ARRAY_BUFFER_ARB, vbo_id);

        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, nullptr);
        glNormalPointer(
                GL_FLOAT, 0, reinterpret_cast<void*>(size * sizeof(Vertex)));
        glTexCoordPointer(2,
                          GL_FLOAT,
                          0,
                          reinterpret_cast<void*>(
                                  size * (sizeof(Vertex) + sizeof(Normal))));
        glDrawArrays(GL_TRIANGLES, 0, size);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    if (use_textures) {
        glBindTexture(GL_TEXTURE_2D, 0);
        if (use_shaders) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
        }
        glDisable(GL_TEXTURE_2D);
    }
    if (use_shaders) {
        glUseProgram(0);
    }
}

bool VBOShaderLibrary::loadShaders(const char* vs_file_name,
                                   const char* fs_file_name) {
    bool shader_status = true;
    use_shaders = true;
    shader_vp = glCreateShader(GL_VERTEX_SHADER);
    shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

    std::ifstream vs_file(vs_file_name, std::ios::binary | std::ios::ate);
    if (vs_file) {
        std::streamsize count = vs_file.tellg();
        vs_file.seekg(0);
        if (count > 0) {
            vs_text.resize(count);
            vs_file.read(vs_text.data(), count);
        } else {
            // shader_status = false;
            // useShaders = false;
        }
    } else {
        // shader_status = false;
        // useShaders = false;
    }

    std::ifstream fs_file(fs_file_name, std::ios::binary | std::ios::ate);
    if (fs_file) {
        std::streamsize count = fs_file.tellg();
        fs_file.seekg(0);
        if (count > 0) {
            fs_text.resize(count);
            fs_file.read(fs_text.data(), count);
        } else {
            // shader_status = false;
            // useShaders = false;
        }
    } else {
        // shader_status = false;
        // useShaders = false;
    }

    /*	ERROR: NO CODE IN SHADERS	*/
    if (vs_text.empty() || fs_text.empty()) {
        cerr << "ERROR: Either vertex shader or fragment shader file not "
                "found."
             << endl;
        // shader_status = false;
        // useShaders = false;
    }

    /*	COMPILE AND VALIDATE THE VERTEX SHADER	*/
    const char* temp_vs_text = vs_text.c_str();
    glShaderSource(shader_vp, 1, &temp_vs_text, nullptr);
    glCompileShader(shader_vp);
    const unsigned int vertex_buffer_size = 2048;
    char buffer1[vertex_buffer_size];
    memset(buffer1, 0, vertex_buffer_size);
    GLsizei length1 = 0;
    glGetShaderInfoLog(shader_vp, vertex_buffer_size, &length1, buffer1);
    if (length1 > 0) {
        cerr << "(" << vs_file_name << ") -- " << buffer1 << endl;
        // shader_status = false;
        // useShaders = false;
    }
    memset(buffer1, 0, vertex_buffer_size);

    /*	COMPILE AND VALIDATE THE FRAGMENT SHADER	*/
    const char* temp_fs_text = fs_text.c_str();
    glShaderSource(shader_fp, 1, &temp_fs_text, nullptr);
    glCompileShader(shader_fp);
    const unsigned int fragment_buffer_size = 2048;
    char buffer2[fragment_buffer_size];
    memset(buffer2, 0, fragment_buffer_size);
    GLsizei length2 = 0;
    glGetShaderInfoLog(shader_fp, fragment_buffer_size, &length2, buffer2);
    if (length2 > 0) {
        cerr << "(" << fs_file_name << ") -- " << buffer2 << endl;
        // shader_status = false;
        // useShaders = false;
    }
    memset(buffer2, 0, fragment_buffer_size);

    /*	COMPILE SHADER INTO ONE AND VALIDATE PROGRAM	*/
    shader_id = glCreateProgram();
    glAttachShader(shader_id, shader_fp);
    glAttachShader(shader_id, shader_vp);
    glLinkProgram(shader_id);
    const unsigned int buffer_size = 512;
    char buffer[buffer_size];
    memset(buffer, 0, buffer_size);
    GLsizei length = 0;
    glGetProgramInfoLog(shader_id, buffer_size, &length, buffer);
    if (length > 0) {
        if (strcmp(buffer,
                   "Fragment shader(s) linked, vertex shader(s) linked.") !=
            0) {  // Handle Radeon Success
            cerr << "Program " << shader_id << " link error: " << buffer
                 << endl;
            // shader_status = false;
            // useShaders = false;
        }
    }
    glValidateProgram(shader_id);
    GLint status;
    glGetProgramiv(shader_id, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        cerr << "Error validating shader " << shader_id << endl;
        // shader_status = false;
        // useShaders = false;
    }
    memset(buffer, 0, buffer_size);

    return shader_status;
}

bool VBOShaderLibrary::loadClientData(const std::string& model_file) {
    bool content_loaded = true;
    unsigned int c;
    bool done = false;

    std::ifstream ogl_file(model_file, std::ios::binary | std::ios::ate);
    if (!ogl_file) {
        printf("ERROR: File %s not found\n", model_file.c_str());
        content_loaded = false;
    } else if (content_loaded && ogl_file) {
        std::streamsize read_file_size = ogl_file.tellg();
        ogl_file.seekg(0);
        string file_content(read_file_size, '\0');
        ogl_file.read(file_content.data(), read_file_size);
        file_content.resize(ogl_file.gcount());

        stringstream strstr(file_content);
        istream_iterator<std::string> token_it(strstr);
        istream_iterator<std::string> token_end;
        vector<string> results(token_it, token_end);

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

            vertices_loaded++;
            line_number += 3;
        }

        vertices.resize(vertices_loaded);
        normals.resize(vertices_loaded);
        tex_coord.resize(vertices_loaded);

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
            vertices[vert_insert_index++] = v;

            ////cout << normal_vec[norm_index + 0] << " " <<
            /// normal_vec[norm_index + 1] << " " << normal_vec[norm_index + 2]
            ///<< endl;
            Normal n(normal_vec[norm_index + 0],
                     normal_vec[norm_index + 1],
                     normal_vec[norm_index + 2]);
            normals[norm_insert_index++] = n;

            ////cout << texcoord_vec[texc_index + 0] << " " <<
            /// texcoord_vec[texc_index + 1] << endl;
            TexCoord t(texcoord_vec[texc_index + 0],
                       texcoord_vec[texc_index + 1]);
            tex_coord[texc_insert_index++] = t;
        }

        try {
            int size = vertices_loaded;
            pgl_gen_buffers_arb(1, &vbo_id);
            pgl_bind_buffer_arb(GL_ARRAY_BUFFER_ARB, vbo_id);
            pgl_buffer_data_arb(GL_ARRAY_BUFFER_ARB,
                                size * (sizeof(Vertex) + sizeof(Normal) +
                                        sizeof(TexCoord)),
                                nullptr,
                                GL_DYNAMIC_DRAW_ARB);
            pgl_buffer_sub_data_arb(GL_ARRAY_BUFFER_ARB,
                                    0,
                                    size * sizeof(Vertex),
                                    vertices.data());
            pgl_buffer_sub_data_arb(GL_ARRAY_BUFFER_ARB,
                                    size * sizeof(Vertex),
                                    size * sizeof(Normal),
                                    normals.data());
            pgl_buffer_sub_data_arb(GL_ARRAY_BUFFER_ARB,
                                    size * (sizeof(Vertex) + sizeof(Normal)),
                                    size * sizeof(TexCoord),
                                    tex_coord.data());
            // cout << "VBOs Initialized" << endl;
            use_vb_os = true;
        } catch (...) {
            cerr << "ERROR: When Loading VBOs" << endl;
            content_loaded = false;
            use_vb_os = false;
        }

        vertices.clear();
        vertices.shrink_to_fit();
        normals.clear();
        normals.shrink_to_fit();
        tex_coord.clear();
        tex_coord.shrink_to_fit();
    }
    return content_loaded;
}

bool VBOShaderLibrary::loadClientData(float* vertex_data,
                                      float* normal_data,
                                      float* tex_coord_data,
                                      int number_of_vertices) {
    bool content_loaded = true;
    try {
        vertices.resize(number_of_vertices);
        normals.resize(number_of_vertices);
        tex_coord.resize(number_of_vertices);

        // cout << "Vertices Data " << endl;
        int vert_index = 0;
        for (int x = 0; x < number_of_vertices * 3; x += 3) {
            Vertex v(vertex_data[x], vertex_data[x + 1], vertex_data[x + 2]);
            vertices[vert_index] = v;
            // cout	<< "<" << vertices[vert_index].coordX << ", "
            //		<< vertices[vert_index].coordY << ", "
            //		<< vertices[vert_index].coordZ
            //		<< ">" << endl;
            vert_index++;
        }
        // cout << "Normal Data" << endl;
        int norm_index = 0;
        for (int y = 0; y < number_of_vertices * 3; y += 3) {
            Normal n(normal_data[y], normal_data[y + 1], normal_data[y + 2]);
            normals[norm_index] = n;
            // cout	<< "<" << normals[norm_index].compoX << ", "
            //		<< normals[norm_index].compoY << ", "
            //		<< normals[norm_index].compoZ
            //		<< ">" << endl;
            norm_index++;
        }
        // cout << "TexCoord Data " << endl;
        int tex_coord_index = 0;
        for (int z = 0; z < number_of_vertices * 2; z += 2) {
            TexCoord t(tex_coord_data[z], tex_coord_data[z + 1]);
            tex_coord[tex_coord_index] = t;
            // cout	<< "<" << tex_coord[tex_coord_index].texcoordS << ", "
            //		<< tex_coord[tex_coord_index].texcoordT
            //		<< ">" << endl;
            tex_coord_index++;
        }
        // cout << endl;

        vertices_loaded = number_of_vertices;
        int size = vertices_loaded;
        pgl_gen_buffers_arb(1, &vbo_id);
        pgl_bind_buffer_arb(GL_ARRAY_BUFFER_ARB, vbo_id);
        pgl_buffer_data_arb(
                GL_ARRAY_BUFFER_ARB,
                size * (sizeof(Vertex) + sizeof(Normal) + sizeof(TexCoord)),
                nullptr,
                GL_DYNAMIC_DRAW_ARB);
        pgl_buffer_sub_data_arb(GL_ARRAY_BUFFER_ARB,
                                0,
                                size * sizeof(Vertex),
                                vertices.data());
        pgl_buffer_sub_data_arb(GL_ARRAY_BUFFER_ARB,
                                size * sizeof(Vertex),
                                size * sizeof(Normal),
                                normals.data());
        pgl_buffer_sub_data_arb(GL_ARRAY_BUFFER_ARB,
                                size * (sizeof(Vertex) + sizeof(Normal)),
                                size * sizeof(TexCoord),
                                tex_coord.data());
        // cout << "VBOs Initialized" << endl;
        use_vb_os = true;
    } catch (...) {
        cerr << "ERROR: When Loading VBOs" << endl;
        content_loaded = false;
        use_vb_os = false;
    }
    vertices.clear();
    vertices.shrink_to_fit();
    normals.clear();
    normals.shrink_to_fit();
    tex_coord.clear();
    tex_coord.shrink_to_fit();
    return content_loaded;
}

void VBOShaderLibrary::swapTexture(const char* filename,
                                   int width,
                                   int height) {
    if (!color_texture) {
        cerr << "ERROR: You must first load a texture" << endl;
    } else {
        glDeleteTextures(1, &color_texture);
        loadTexture(filename, width, height);
    }
}

void VBOShaderLibrary::loadTexture(const char* filename,
                                   int width,
                                   int height) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        // cout << "ERROR: File Does Not Exist Not Going to Use Textures" <<
        // endl;
    } else {
        std::vector<unsigned char> data(width * height * 3);
        file.read(reinterpret_cast<char*>(data.data()), data.size());
        glGenTextures(1, &(color_texture));
        glBindTexture(GL_TEXTURE_2D, color_texture);
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
                     data.data());
        use_textures = true;
        // cout << "Done Loading Image" << endl;
    }
}

void VBOShaderLibrary::swapTextureNormals(const char* filename,
                                          int width,
                                          int height) {
    if (!color_texture) {
        cerr << "ERROR: You must first load a normal texture" << endl;
    } else {
        glDeleteTextures(1, &color_texture);
        loadTexture(filename, width, height);
    }
}

void VBOShaderLibrary::loadTextureNormals(const char* filename,
                                          int width,
                                          int height) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        cerr << "ERROR: File Does Not Exist Not Going to Use Texture Normals"
             << endl;
    } else {
        std::vector<unsigned char> data(width * height * 3);
        file.read(reinterpret_cast<char*>(data.data()), data.size());
        glGenTextures(1, &(normal_texture));
        glBindTexture(GL_TEXTURE_2D, normal_texture);
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
                     data.data());
        use_textures = true;
        // cout << "Done Loading Image" << endl;
    }
}

bool VBOShaderLibrary::getVBOPointerFunctions() {
    bool status = true;
    pgl_gen_buffers_arb =
            reinterpret_cast<PFNGLGENBUFFERSARBPROC>(glXGetProcAddress(
                    reinterpret_cast<const GLubyte*>("glGenBuffersARB")));
    pgl_bind_buffer_arb =
            reinterpret_cast<PFNGLBINDBUFFERARBPROC>(glXGetProcAddress(
                    reinterpret_cast<const GLubyte*>("glBindBufferARB")));
    pgl_buffer_data_arb =
            reinterpret_cast<PFNGLBUFFERDATAARBPROC>(glXGetProcAddress(
                    reinterpret_cast<const GLubyte*>("glBufferDataARB")));
    pgl_buffer_sub_data_arb =
            reinterpret_cast<PFNGLBUFFERSUBDATAARBPROC>(glXGetProcAddress(
                    reinterpret_cast<const GLubyte*>("glBufferSubDataARB")));
    pgl_delete_buffers_arb =
            reinterpret_cast<PFNGLDELETEBUFFERSARBPROC>(glXGetProcAddress(
                    reinterpret_cast<const GLubyte*>("glDeleteBuffersARB")));
    pgl_get_buffer_parameteriv_arb =
            reinterpret_cast<PFNGLGETBUFFERPARAMETERIVARBPROC>(
                    glXGetProcAddress(reinterpret_cast<const GLubyte*>(
                            "glGetBufferParameterivARB")));
    pgl_map_buffer_arb =
            reinterpret_cast<PFNGLMAPBUFFERARBPROC>(glXGetProcAddress(
                    reinterpret_cast<const GLubyte*>("glMapBufferARB")));
    pgl_unmap_buffer_arb =
            reinterpret_cast<PFNGLUNMAPBUFFERARBPROC>(glXGetProcAddress(
                    reinterpret_cast<const GLubyte*>("glUnmapBufferARB")));
    if (!pgl_gen_buffers_arb || !pgl_bind_buffer_arb || !pgl_buffer_data_arb ||
        !pgl_buffer_sub_data_arb || !pgl_delete_buffers_arb ||
        !pgl_get_buffer_parameteriv_arb || !pgl_map_buffer_arb ||
        !pgl_unmap_buffer_arb) {
        printf("ERROR: VBO Initializtion Failed");
        status = false;
    }
    if (status) {
        // printf("VBOs Available for Object %x\n",this);
    }
    return status;
}

/*	STATIC FUNCTION */
bool VBOShaderLibrary::initGlew() {
    bool init_ok = false;
    GLenum glew_err = glewInit();
    if (GLEW_OK != glew_err) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_err));
    } else {
        // fprintf(stdout, "Status: Using GLEW (%s)\n",
        // glewGetString(GLEW_VERSION));
        init_ok = true;
    }
    return init_ok;
}

/*	STATIC FUNCTION */
bool VBOShaderLibrary::areVbOsSupported() {
    bool qualified = true;

    const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    if (!vendor) qualified = false;

    const char* renderer =
            reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    if (!renderer) qualified = false;

    const char* version =
            reinterpret_cast<const char*>(glGetString(GL_VERSION));
    if (!version) qualified = false;

    const char* extensions_raw =
            reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    if (!extensions_raw) qualified = false;

    std::vector<std::string> extensions;
    if (extensions_raw) {
        std::string extensions_str = extensions_raw;
        std::string current;
        for (char ch : extensions_str) {
            if (ch != ' ') {
                current += ch;
            } else if (!current.empty()) {
                extensions.push_back(current);
                current.clear();
            }
        }
        if (!current.empty()) {
            extensions.push_back(current);
        }
    }

    int red_bits, green_bits, blue_bits, alpha_bits, depth_bits, stencil_bits;
    int max_texture_size, max_lights, max_attrib_stacks, max_model_view_stacks;
    int max_projection_stacks, max_clip_planes, max_texture_stacks;
    glGetIntegerv(GL_RED_BITS, &red_bits);
    glGetIntegerv(GL_GREEN_BITS, &green_bits);
    glGetIntegerv(GL_BLUE_BITS, &blue_bits);
    glGetIntegerv(GL_ALPHA_BITS, &alpha_bits);
    glGetIntegerv(GL_DEPTH_BITS, &depth_bits);
    glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
    glGetIntegerv(GL_MAX_LIGHTS, &max_lights);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    glGetIntegerv(GL_MAX_CLIP_PLANES, &max_clip_planes);
    glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &max_model_view_stacks);
    glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &max_projection_stacks);
    glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH, &max_attrib_stacks);
    glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &max_texture_stacks);

    const std::string exten = "GL_ARB_vertex_buffer_object";
    bool extension_exists = false;
    for (const std::string& compare : extensions) {
        if (compare == exten) {
            extension_exists = true;
        }
    }
    if (!extension_exists) {
        qualified = false;
    }

    if (qualified) {
        // fprintf(stdout, "Status: GL_ARB VBOs supported (%s)\n", version);
    }

    return qualified;
}
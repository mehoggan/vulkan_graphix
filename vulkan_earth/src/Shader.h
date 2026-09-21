#ifndef __SHADER_H
#define __SHADER_H
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdlib.h>
#include <string>

class Shader {
public:
    Shader();
    Shader(const char* vs_file, const char* fs_file);
    ~Shader();
    void validateShader(GLuint shader, const char* file = nullptr);
    void validateProgram(GLuint program);

    void init(const char* vs_file, const char* fs_file);
    void bind();
    void unbind();

    unsigned int id();

private:
    unsigned int shader_id;
    unsigned int shader_vp;
    unsigned int shader_fp;
    std::string textFileRead(const char* file_name);
};
#endif /*__SHADER_H__*/
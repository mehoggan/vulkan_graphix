#ifndef __SHADER_H
#define __SHADER_H
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdlib.h>
#include <string>

#pragma comment(lib, "glew32.lib")

class Shader {
public:
    Shader();
    Shader(const char* vsFile, const char* fsFile);
    ~Shader();
    void validateShader(GLuint shader, const char* file = nullptr);
    void validateProgram(GLuint program);

    void init(const char* vsFile, const char* fsFile);
    void bind();
    void unbind();

    unsigned int id();

private:
    char* text;
    unsigned int shader_id;
    unsigned int shader_vp;
    unsigned int shader_fp;
    char* textFileRead(const char* fileName);
};
#endif /*__SHADER_H__*/
#include "Shader.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include "macro_crtdbg.h"

using namespace std;

Shader::Shader() = default;

Shader::Shader(const char* vsFile, const char* fsFile) {
    init(vsFile, fsFile);
}

Shader::~Shader() {
    glDetachShader(this->shader_id, this->shader_fp);
    glDetachShader(this->shader_id, this->shader_vp);

    glDeleteShader(this->shader_fp);
    glDeleteShader(this->shader_vp);
    glDeleteProgram(this->shader_id);
}

void Shader::validateShader(GLuint shader, const char* file) {
    const unsigned int buffer_size = 512;
    char buffer[buffer_size];
    memset(buffer, 0, buffer_size);
    GLsizei length = 0;
    glGetShaderInfoLog(shader, buffer_size, &length, buffer);
    if (length > 0) {
        cerr << "(" << file << ") -- " << buffer << endl;
        // int pause;
        // cin >> pause;
        // exit(0);
    }
}

void Shader::validateProgram(GLuint program) {
    const unsigned int buffer_size = 512;
    char buffer[buffer_size];
    memset(buffer, 0, buffer_size);
    GLsizei length = 0;

    memset(buffer, 0, buffer_size);
    glGetProgramInfoLog(program, buffer_size, &length, buffer);
    if (length > 0) {
        cerr << "Program " << program << " link error: " << buffer << endl;
        // int pause;
        // cin >> pause;
        // exit(0);
    }

    glValidateProgram(program);
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        cerr << "Error validating shader " << program << endl;
        // int pause;
        // cin >> pause;
        // exit(0);
    }
}

void Shader::init(const char* vsFile, const char* fsFile) {
    this->shader_vp = glCreateShader(GL_VERTEX_SHADER);
    this->shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vs_text_str = textFileRead(vsFile);
    std::string fs_text_str = textFileRead(fsFile);

    if (vs_text_str.empty() || fs_text_str.empty()) {
        cerr << "Either vertex shader or fragment shader file not found."
             << endl;
        exit(0);
    }

    const char* vs_text = vs_text_str.c_str();
    glShaderSource(this->shader_vp, 1, &vs_text, nullptr);
    glCompileShader(this->shader_vp);
    validateShader(shader_vp, vsFile);

    const char* fs_text = fs_text_str.c_str();
    glShaderSource(this->shader_fp, 1, &fs_text, nullptr);
    glCompileShader(this->shader_fp);
    this->validateShader(shader_fp, fsFile);

    this->shader_id = glCreateProgram();
    glAttachShader(this->shader_id, this->shader_fp);
    glAttachShader(this->shader_id, this->shader_vp);
    glLinkProgram(this->shader_id);
    validateProgram(shader_id);
}

unsigned int Shader::id() { return this->shader_id; }

void Shader::bind() { glUseProgram(this->shader_id); }

void Shader::unbind() { glUseProgram(0); }

std::string Shader::textFileRead(const char* fileName) {
    std::string text;
    if (fileName == nullptr) {
        exit(0);
    }
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if (file) {
        std::streamsize count = file.tellg();
        file.seekg(0);
        if (count > 0) {
            text.resize(count);
            file.read(text.data(), count);
        }
        return text;
    }
    exit(0);
}

#include "Shader.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include "macro_crtdbg.h"

using namespace std;

Shader::Shader() = default;

Shader::Shader(const char* vs_file, const char* fs_file) {
    init(vs_file, fs_file);
}

Shader::~Shader() {
    glDetachShader(shader_id, shader_fp);
    glDetachShader(shader_id, shader_vp);

    glDeleteShader(shader_fp);
    glDeleteShader(shader_vp);
    glDeleteProgram(shader_id);
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

void Shader::init(const char* vs_file, const char* fs_file) {
    shader_vp = glCreateShader(GL_VERTEX_SHADER);
    shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vs_text_str = textFileRead(vs_file);
    std::string fs_text_str = textFileRead(fs_file);

    if (vs_text_str.empty() || fs_text_str.empty()) {
        cerr << "Either vertex shader or fragment shader file not found."
             << endl;
        exit(0);
    }

    const char* vs_text = vs_text_str.c_str();
    glShaderSource(shader_vp, 1, &vs_text, nullptr);
    glCompileShader(shader_vp);
    validateShader(shader_vp, vs_file);

    const char* fs_text = fs_text_str.c_str();
    glShaderSource(shader_fp, 1, &fs_text, nullptr);
    glCompileShader(shader_fp);
    validateShader(shader_fp, fs_file);

    shader_id = glCreateProgram();
    glAttachShader(shader_id, shader_fp);
    glAttachShader(shader_id, shader_vp);
    glLinkProgram(shader_id);
    validateProgram(shader_id);
}

unsigned int Shader::id() { return shader_id; }

void Shader::bind() { glUseProgram(shader_id); }

void Shader::unbind() { glUseProgram(0); }

std::string Shader::textFileRead(const char* file_name) {
    std::string text;
    if (file_name == nullptr) {
        exit(0);
    }
    std::ifstream file(file_name, std::ios::binary | std::ios::ate);
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

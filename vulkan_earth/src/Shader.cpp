#include <cstring>
#include "Shader.h"
#include <iostream>
#include "macro_crtdbg.h"


using namespace std;

Shader::Shader() {
}

Shader::Shader(const char *vsFile, const char *fsFile) {
    init(vsFile, fsFile);
}

Shader::~Shader() {
	if(text) {
		delete [] text;
	}
	glDetachShader(this->shader_id, this->shader_fp);
    glDetachShader(this->shader_id, this->shader_vp);
    
    glDeleteShader(this->shader_fp);
    glDeleteShader(this->shader_vp);
    glDeleteProgram(this->shader_id);
}

void Shader::validateShader(GLuint shader, const char* file) {
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;
	glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
    if (length > 0) {
		cerr << "(" << file << ") -- " << buffer << endl;
		//int pause;
		//cin >> pause;
		//exit(0);
    }
}

void Shader::validateProgram(GLuint program) {
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;
    
    memset(buffer, 0, BUFFER_SIZE);
    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
	if (length > 0) {
        cerr << "Program " << program << " link error: " << buffer << endl;
		//int pause;
		//cin >> pause;
		//exit(0);
	}
    
    glValidateProgram(program);
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE) {
        cerr << "Error validating shader " << program << endl;
		//int pause;
		//cin >> pause;
		//exit(0);
	}
}



void Shader::init(const char *vsFile, const char *fsFile) {
	this->shader_vp = glCreateShader(GL_VERTEX_SHADER);
    this->shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vsText = textFileRead(vsFile);
    const char* fsText = textFileRead(fsFile);

	if (vsText == NULL || fsText == NULL) {
		cerr << "Either vertex shader or fragment shader file not found." << endl;
		exit(0);
    }

	glShaderSource(this->shader_vp, 1, &vsText, 0);
	glCompileShader(this->shader_vp);
	validateShader(shader_vp, vsFile);

	glShaderSource(this->shader_fp, 1, &fsText, 0);
    glCompileShader(this->shader_fp);
	this->validateShader(shader_fp, fsFile);

	this->shader_id = glCreateProgram();
    glAttachShader(this->shader_id, this->shader_fp);
    glAttachShader(this->shader_id, this->shader_vp);
    glLinkProgram(this->shader_id);
	validateProgram(shader_id);
	fsText = NULL;
	vsText = NULL;
	delete [] this->text;
	this->text = NULL;
}

unsigned int Shader::id() {
    return this->shader_id;
}

void Shader::bind() {
    glUseProgram(this->shader_id);
}

void Shader::unbind() {
    glUseProgram(0);
}

char* Shader::textFileRead(const char *fileName) {
	text=NULL;
	if (fileName != NULL) {
		FILE *file;
		file = fopen(fileName, "rt");
		if (file != NULL) {
			fseek(file, 0, SEEK_END);
			int count = ftell(file);
			rewind(file);
			if (count > 0) {
				//text = (char*)malloc(sizeof(char) * (count + 1));
				text = new char[count + 1];
				count = fread(text, sizeof(char), count, file);
				text[count] = '\0';
			}
			fclose(file);
			return text;
		}
	}else {
		exit(0);
	}
	if(text == NULL){
		exit(0);
	}else {
		return text;
	}
}


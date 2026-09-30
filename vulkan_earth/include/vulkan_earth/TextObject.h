#ifndef TEXT_OBJECT_H
#define TEXT_OBJECT_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>

class TextObject {
public:
    TextObject();
    TextObject(const std::string& input,
               GLfloat new_pos_x,
               GLfloat new_pos_y,
               GLfloat new_pos_z,
               void* new_font_size,
               GLfloat red,
               GLfloat green,
               GLfloat blue);
    ~TextObject();
    const std::string& getOutput();
    void setXpos(GLfloat x);
    void setYpos(GLfloat y);
    void setZpos(GLfloat z);
    GLvoid draw();

private:
    std::string output;
    GLfloat pos_x;
    GLfloat pos_y;
    GLfloat pos_z;
    GLfloat color[4];
    void* font_size;
};

#endif

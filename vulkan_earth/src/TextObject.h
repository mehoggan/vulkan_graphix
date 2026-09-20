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
               GLfloat posX,
               GLfloat posY,
               GLfloat posZ,
               void* font_size,
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
    GLfloat posX;
    GLfloat posY;
    GLfloat posZ;
    GLfloat color[4];
    void* font_size;
};

#endif

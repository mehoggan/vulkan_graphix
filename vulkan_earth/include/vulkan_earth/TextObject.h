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
               float new_pos_x,
               float new_pos_y,
               float new_pos_z,
               void* new_font_size,
               float red,
               float green,
               float blue);
    ~TextObject();
    const std::string& getOutput();
    void setXpos(float x);
    void setYpos(float y);
    void setZpos(float z);
    void draw();

private:
    std::string output;
    float pos_x;
    float pos_y;
    float pos_z;
    float color[4];
    void* font_size;
};

#endif

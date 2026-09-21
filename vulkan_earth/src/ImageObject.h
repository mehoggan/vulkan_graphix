#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include "stdio.h"

#ifndef IMAGEOBJECT
#define IMAGEOBJECT

class ImageObject {
public:
    ImageObject();
    ImageObject(GLfloat x_pos,
                GLfloat y_pos,
                GLfloat z_pos,
                GLint width,
                GLint height,
                GLfloat border,
                GLint iwidth,
                GLint iheight,
                const std::string& filename);
    ~ImageObject();
    GLfloat getXpos();
    GLfloat getYpos();
    GLfloat getZpos();
    GLint getWidth();
    GLint getHeight();
    void setXpos(GLfloat x);
    void setYpos(GLfloat y);
    void setZpos(GLfloat z);
    void setWidth(GLint w);
    void setHeight(GLint h);
    void draw();

private:
    GLfloat x_pos;
    GLfloat y_pos;
    GLfloat z_pos;
    GLint width;
    GLint height;
    GLfloat border_size;
    GLuint texture;
};
#endif
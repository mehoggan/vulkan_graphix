#include "ImageObject.h"
#include <fstream>
#include <vector>
#include "MacroCrtdbg.h"

ImageObject::ImageObject() = default;

ImageObject::ImageObject(GLfloat new_x_pos,
                         GLfloat new_y_pos,
                         GLfloat new_z_pos,
                         GLint new_width,
                         GLint new_height,
                         GLfloat border,
                         GLint i_width,
                         GLint i_height,
                         const std::string& filename) {
    x_pos = new_x_pos;
    y_pos = new_y_pos;
    z_pos = new_z_pos;
    width = new_width;
    height = new_height;
    border_size = border;

    int img_width = i_width;
    int img_height = i_height;

    std::vector<unsigned char> data(img_width * img_height * 3);
    std::ifstream file(filename, std::ios::binary);
    file.read(reinterpret_cast<char*>(data.data()), data.size());
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 img_width,
                 img_height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 data.data());
}

ImageObject::~ImageObject() { glDeleteTextures(1, &texture); }

/*GETTERS & SETTERS*/
GLfloat ImageObject::getXpos() { return x_pos; }
GLfloat ImageObject::getYpos() { return y_pos; }
GLfloat ImageObject::getZpos() { return z_pos; }
GLint ImageObject::getWidth() { return width; }
GLint ImageObject::getHeight() { return height; }
void ImageObject::setXpos(GLfloat x) { x_pos = x; }
void ImageObject::setYpos(GLfloat y) { y_pos = y; }
void ImageObject::setZpos(GLfloat z) { z_pos = z; }
void ImageObject::setWidth(GLint w) { width = w; }
void ImageObject::setHeight(GLint h) { height = h; }

void ImageObject::draw() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glPushMatrix();
    // image plane
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex3f(x_pos, y_pos, z_pos);
    glTexCoord2f(0, 0);
    glVertex3f(x_pos, y_pos - height, z_pos);
    glTexCoord2f(1, 0);
    glVertex3f(x_pos + width, y_pos - height, z_pos);
    glTexCoord2f(1, 1);
    glVertex3f(x_pos + width, y_pos, z_pos);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    if (border_size != 0) {
        // top and right borders
        glBegin(GL_QUADS);
        glColor3f(0.45, 0.45, 0.45);

        glVertex3f(x_pos, y_pos, z_pos);
        glVertex3f(x_pos - border_size, y_pos + border_size, z_pos);
        glVertex3f(x_pos + width + border_size, y_pos + border_size, z_pos);
        glVertex3f(x_pos + width, y_pos, z_pos);

        glVertex3f(x_pos - border_size, y_pos + border_size, z_pos);
        glVertex3f(x_pos - border_size, y_pos - height - border_size, z_pos);
        glVertex3f(x_pos, y_pos - height, z_pos);
        glVertex3f(x_pos, y_pos, z_pos);
        glEnd();

        // bottom and left borders
        glBegin(GL_QUADS);
        glColor3f(0.85, 0.85, 0.85);

        glVertex3f(x_pos - border_size, y_pos - height - border_size, z_pos);
        glVertex3f(x_pos + width + border_size,
                   y_pos - height - border_size,
                   z_pos);
        glVertex3f(x_pos + width, y_pos - height, z_pos);
        glVertex3f(x_pos, y_pos - height, z_pos);

        glVertex3f(x_pos + width, y_pos, z_pos);
        glVertex3f(x_pos + width + border_size, y_pos + border_size, z_pos);
        glVertex3f(x_pos + width + border_size,
                   y_pos - height - border_size,
                   z_pos);
        glVertex3f(x_pos + width, y_pos + -height, z_pos);
        glEnd();
    }
    glPopMatrix();
}
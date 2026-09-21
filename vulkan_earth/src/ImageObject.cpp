#include "ImageObject.h"
#include <fstream>
#include <vector>
#include "macro_crtdbg.h"

ImageObject::ImageObject() = default;

ImageObject::ImageObject(GLfloat x_pos,
                         GLfloat y_pos,
                         GLfloat z_pos,
                         GLint width,
                         GLint height,
                         GLfloat border,
                         GLint i_width,
                         GLint i_height,
                         const std::string& filename) {
    this->x_pos = x_pos;
    this->y_pos = y_pos;
    this->z_pos = z_pos;
    this->width = width;
    this->height = height;
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
GLfloat ImageObject::getXpos() { return this->x_pos; }
GLfloat ImageObject::getYpos() { return this->y_pos; }
GLfloat ImageObject::getZpos() { return this->z_pos; }
GLint ImageObject::getWidth() { return this->width; }
GLint ImageObject::getHeight() { return this->height; }
void ImageObject::setXpos(GLfloat x) { this->x_pos = x; }
void ImageObject::setYpos(GLfloat y) { this->y_pos = y; }
void ImageObject::setZpos(GLfloat z) { this->z_pos = z; }
void ImageObject::setWidth(GLint w) { this->width = w; }
void ImageObject::setHeight(GLint h) { this->height = h; }

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
    glVertex3f(this->x_pos, this->y_pos, this->z_pos);
    glTexCoord2f(0, 0);
    glVertex3f(this->x_pos, this->y_pos - height, this->z_pos);
    glTexCoord2f(1, 0);
    glVertex3f(this->x_pos + width, this->y_pos - height, this->z_pos);
    glTexCoord2f(1, 1);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    if (border_size != 0) {
        // top and right borders
        glBegin(GL_QUADS);
        glColor3f(0.45, 0.45, 0.45);

        glVertex3f(this->x_pos, this->y_pos, this->z_pos);
        glVertex3f(this->x_pos - border_size,
                   this->y_pos + border_size,
                   this->z_pos);
        glVertex3f(this->x_pos + width + border_size,
                   this->y_pos + border_size,
                   this->z_pos);
        glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);

        glVertex3f(this->x_pos - border_size,
                   this->y_pos + border_size,
                   this->z_pos);
        glVertex3f(this->x_pos - border_size,
                   this->y_pos - height - border_size,
                   this->z_pos);
        glVertex3f(this->x_pos, this->y_pos - height, this->z_pos);
        glVertex3f(this->x_pos, this->y_pos, this->z_pos);
        glEnd();

        // bottom and left borders
        glBegin(GL_QUADS);
        glColor3f(0.85, 0.85, 0.85);

        glVertex3f(this->x_pos - border_size,
                   this->y_pos - height - border_size,
                   this->z_pos);
        glVertex3f(this->x_pos + width + border_size,
                   this->y_pos - height - border_size,
                   this->z_pos);
        glVertex3f(this->x_pos + width, this->y_pos - height, this->z_pos);
        glVertex3f(this->x_pos, this->y_pos - height, this->z_pos);

        glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
        glVertex3f(this->x_pos + width + border_size,
                   this->y_pos + border_size,
                   this->z_pos);
        glVertex3f(this->x_pos + width + border_size,
                   this->y_pos - height - border_size,
                   this->z_pos);
        glVertex3f(this->x_pos + width, this->y_pos + -height, this->z_pos);
        glEnd();
    }
    glPopMatrix();
}
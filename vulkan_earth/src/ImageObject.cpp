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
    this->xPos = x_pos;
    this->yPos = y_pos;
    this->zPos = z_pos;
    this->width = width;
    this->height = height;
    borderSize = border;

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
GLfloat ImageObject::getXpos() { return this->xPos; }
GLfloat ImageObject::getYpos() { return this->yPos; }
GLfloat ImageObject::getZpos() { return this->zPos; }
GLint ImageObject::getWidth() { return this->width; }
GLint ImageObject::getHeight() { return this->height; }
void ImageObject::setXpos(GLfloat x) { this->xPos = x; }
void ImageObject::setYpos(GLfloat y) { this->yPos = y; }
void ImageObject::setZpos(GLfloat z) { this->zPos = z; }
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
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glTexCoord2f(0, 0);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glTexCoord2f(1, 0);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glTexCoord2f(1, 1);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    if (borderSize != 0) {
        // top and right borders
        glBegin(GL_QUADS);
        glColor3f(0.45, 0.45, 0.45);

        glVertex3f(this->xPos, this->yPos, this->zPos);
        glVertex3f(
                this->xPos - borderSize, this->yPos + borderSize, this->zPos);
        glVertex3f(this->xPos + width + borderSize,
                   this->yPos + borderSize,
                   this->zPos);
        glVertex3f(this->xPos + width, this->yPos, this->zPos);

        glVertex3f(
                this->xPos - borderSize, this->yPos + borderSize, this->zPos);
        glVertex3f(this->xPos - borderSize,
                   this->yPos - height - borderSize,
                   this->zPos);
        glVertex3f(this->xPos, this->yPos - height, this->zPos);
        glVertex3f(this->xPos, this->yPos, this->zPos);
        glEnd();

        // bottom and left borders
        glBegin(GL_QUADS);
        glColor3f(0.85, 0.85, 0.85);

        glVertex3f(this->xPos - borderSize,
                   this->yPos - height - borderSize,
                   this->zPos);
        glVertex3f(this->xPos + width + borderSize,
                   this->yPos - height - borderSize,
                   this->zPos);
        glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
        glVertex3f(this->xPos, this->yPos - height, this->zPos);

        glVertex3f(this->xPos + width, this->yPos, this->zPos);
        glVertex3f(this->xPos + width + borderSize,
                   this->yPos + borderSize,
                   this->zPos);
        glVertex3f(this->xPos + width + borderSize,
                   this->yPos - height - borderSize,
                   this->zPos);
        glVertex3f(this->xPos + width, this->yPos + -height, this->zPos);
        glEnd();
    }
    glPopMatrix();
}
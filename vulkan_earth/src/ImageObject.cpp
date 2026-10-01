#include "vulkan_earth/ImageObject.h"
#include <cstdint>
#include <fstream>
#include <vector>
#include "vulkan_earth/MacroCrtdbg.h"

ImageObject::ImageObject() = default;

ImageObject::ImageObject(float new_x_pos,
                         float new_y_pos,
                         float new_z_pos,
                         std::int32_t new_width,
                         std::int32_t new_height,
                         float border,
                         std::int32_t i_width,
                         std::int32_t i_height,
                         const std::string& filename) {
    x_pos = new_x_pos;
    y_pos = new_y_pos;
    z_pos = new_z_pos;
    width = new_width;
    height = new_height;
    border_size = border;

    std::int32_t img_width = i_width;
    std::int32_t img_height = i_height;

    std::vector<std::uint8_t> data(img_width * img_height * 3);
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
float ImageObject::getXpos() { return x_pos; }
float ImageObject::getYpos() { return y_pos; }
float ImageObject::getZpos() { return z_pos; }
std::int32_t ImageObject::getWidth() { return width; }
std::int32_t ImageObject::getHeight() { return height; }
void ImageObject::setXpos(float x) { x_pos = x; }
void ImageObject::setYpos(float y) { y_pos = y; }
void ImageObject::setZpos(float z) { z_pos = z; }
void ImageObject::setWidth(std::int32_t w) { width = w; }
void ImageObject::setHeight(std::int32_t h) { height = h; }

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
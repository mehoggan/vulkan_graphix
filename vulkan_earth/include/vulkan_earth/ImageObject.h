#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>
#include <string>
#include "stdio.h"

#ifndef IMAGEOBJECT
#define IMAGEOBJECT

class ImageObject {
public:
    ImageObject();
    ImageObject(float new_x_pos,
                float new_y_pos,
                float new_z_pos,
                std::int32_t new_width,
                std::int32_t new_height,
                float border,
                std::int32_t i_width,
                std::int32_t i_height,
                const std::string& filename);
    ~ImageObject();
    float getXpos();
    float getYpos();
    float getZpos();
    std::int32_t getWidth();
    std::int32_t getHeight();
    void setXpos(float x);
    void setYpos(float y);
    void setZpos(float z);
    void setWidth(std::int32_t w);
    void setHeight(std::int32_t h);
    void draw();

private:
    float x_pos;
    float y_pos;
    float z_pos;
    std::int32_t width;
    std::int32_t height;
    float border_size;
    std::uint32_t texture;
};
#endif
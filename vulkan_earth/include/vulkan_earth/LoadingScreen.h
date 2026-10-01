#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <cstdint>
#include <iostream>

class ImageObject;

class LoadingScreen {
public:
    LoadingScreen();
    LoadingScreen(float x,
                  float y,
                  float z,
                  std::int32_t new_width,
                  std::int32_t new_height,
                  float red,
                  float green,
                  float blue,
                  float alpha);
    ~LoadingScreen();
    void draw();

private:
    float pos[3];
    float color[4];
    std::int32_t width, height;
    ImageObject* image;
};
#endif
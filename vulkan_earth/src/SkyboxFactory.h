#ifndef SKYBOX_FACTORY_H_
#define SKYBOX_FACTORY_H_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>

class SkyboxFactory {
public:
    SkyboxFactory();
    SkyboxFactory(int size_of_box);
    ~SkyboxFactory();
    void draw();
    void printData();

private:
    GLuint texture0;
    GLuint texture1;
    GLuint texture2;
    GLuint texture3;
    GLuint texture4;
    GLuint texture5;
    GLuint texture6;
    float size;
    unsigned char* data0;
    unsigned char* data1;
    unsigned char* data2;
    unsigned char* data3;
    unsigned char* data4;
    unsigned char* data5;
    unsigned char* data6;
    FILE* file0;
    int image1_X_start;
    int image1_Y_start;
    int image1_X_end;
    int image1_Y_end;
    int image2_X_start;
    int image2_Y_start;
    int image2_X_end;
    int image2_Y_end;
    int image3_X_start;
    int image3_Y_start;
    int image3_X_end;
    int image3_Y_end;
    int image4_X_start;
    int image4_Y_start;
    int image4_X_end;
    int image4_Y_end;
    int image5_X_start;
    int image5_Y_start;
    int image5_X_end;
    int image5_Y_end;
    int image6_X_start;
    int image6_Y_start;
    int image6_X_end;
    int image6_Y_end;
};

#endif /* SKYBOX_FACTORY_H_ */
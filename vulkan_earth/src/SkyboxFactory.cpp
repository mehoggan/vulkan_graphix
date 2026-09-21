#include "SkyboxFactory.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include "macro_crtdbg.h"

SkyboxFactory::SkyboxFactory() = default;

SkyboxFactory::SkyboxFactory(int size_of_box) {
    size = size_of_box;
    int width = 1024;
    int height = 1024;
    int scale = 1024;
    std::vector<unsigned char> data0(width * height * 3);
    std::ifstream file0("SkyBox.raw", std::ios::binary);
    if (!file0) {
        std::cerr << "ERROR: File Not Found" << std::endl;
        exit(0);
    }
    file0.read(reinterpret_cast<char*>(data0.data()), data0.size());

    /*
    image1_X_start=(int)((width*3)/4);
    image1_Y_start=(int)(0);
    image1_X_end=(int)((width*3)/2);
    image1_Y_end=(int)((height)/3);

    image2_X_start=(int)(0);
    image2_Y_start=(int)((height*3)/3);
    image2_X_end=(int)((width*3)/4);
    image2_Y_end=(int)(2*(height)/3);

    image3_X_start=(int)((width*3)/4);
    image3_Y_start=(int)((height)/3);
    image3_X_end=(int)((width*3)/2);
    image3_Y_end=(int)(2*(height)/3);

    image4_X_start=(int)((width*3)/2);
    image4_Y_start=(int)((height)/3);
    image4_X_end=(int)(3*(width*3)/4);
    image4_Y_end=(int)(2*(height)/3);

    image5_X_start=(int)(3*(width*3)/4);
    image5_Y_start=(int)((height)/3);
    image5_X_end=(int)(width*3);
    image5_Y_end=(int)(2*(height)/3);

    image6_X_start=(int)((width*3)/4);
    image6_Y_start=(int)(2*(height)/3);
    image6_X_end=(int)((width*3)/2);
    image6_Y_end=(int)(height);

    int size_of_data = width*height*3;
    int index1=0;
    int index2=0;
    int index3=0;
    int index4=0;
    int index5=0;
    int index6=0;
    for(int i = 0; i < width*height; i+=3) {
        if	(
                (i/(size_of_data) >= image1_Y_start && i/(size_of_data) <
    image1_Y_end) && (i%(size_of_data) >= image1_X_start && i%(size_of_data) <
    image1_X_end) ) {
            //COLLECT DATA FOR FIRST IMAGE
            data1[index1 + 0]=data0[i];
            data1[index1 + 1]=data0[i + 1];
            data1[index1 + 2]=data0[i + 2];
            index1+=3;
        }else if (
                (i/(size_of_data) >= image2_Y_start && i/(size_of_data) <
    image2_Y_end) && (i%(size_of_data) >= image2_X_start && i%(size_of_data) <
    image2_X_end) ) {
            //COLLECT DATA FOR SECOND IMAGE
            data2[index2 + 0]=data0[i];
            data2[index2 + 1]=data0[i + 1];
            data2[index2 + 2]=data0[i + 2];
            index2+=3;
        }else if (
                (i/(size_of_data) >= image3_Y_start && i/(size_of_data) <
    image3_Y_end) && (i%(size_of_data) >= image3_X_start && i%(size_of_data) <
    image3_X_end) ) {
            //COLLECT DATA FOR THIRD IMAGE
            data3[index3 + 0]=data0[i];
            data3[index3 + 1]=data0[i + 1];
            data3[index3 + 2]=data0[i + 2];
            index3+=3;
        }else if (
                (i/(size_of_data) >= image4_Y_start && i/(size_of_data) <
    image4_Y_end) && (i%(size_of_data) >= image4_X_start && i%(size_of_data) <
    image4_X_end) ) {
            //COLLECT DATA FOR FOURTH IMAGE
            data4[index4 + 0]=data0[i];
            data4[index4 + 1]=data0[i + 1];
            data4[index4 + 2]=data0[i + 2];
            index4+=3;
        }else if (
                (i/(size_of_data) >= image5_Y_start && i/(size_of_data) <
    image5_Y_end) && (i%(size_of_data) >= image5_X_start && i%(size_of_data) <
    image5_X_end) ) {
            //COLLECT DATA FOR FIFTH IMAGE
            data5[index5 + 0]=data0[i];
            data5[index5 + 1]=data0[i + 1];
            data5[index5 + 2]=data0[i + 2];
            index5+=3;
        }else if (
                (i/(size_of_data) >= image6_Y_start && i/(size_of_data) <
    image6_Y_end) && (i%(size_of_data) >= image6_X_start && i%(size_of_data) <
    image6_X_end) ) {
            //COLLECT DATA FOR SIXTH IMAGE
            data6[index6 + 0]=data0[i];
            data6[index6 + 1]=data0[i + 1];
            data6[index6 + 2]=data0[i + 2];
            index6+=3;
        }
    }
    //*/

    glGenTextures(1, &texture0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 width,
                 height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 data0.data());

    /*
    glGenTextures(1,&texture1);
    glBindTexture(GL_TEXTURE_2D,texture1);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)(width/4), (int)(height/3),
    0,GL_RGB, GL_UNSIGNED_BYTE, data1);

    glGenTextures(1,&texture2);
    glBindTexture(GL_TEXTURE_2D,texture2);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)(width/4), (int)(height/3),
    0,GL_RGB, GL_UNSIGNED_BYTE, data2);

    glGenTextures(1,&texture3);
    glBindTexture(GL_TEXTURE_2D,texture3);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)(width/4), (int)(height/3),
    0,GL_RGB, GL_UNSIGNED_BYTE, data3);

    glGenTextures(1,&texture4);
    glBindTexture(GL_TEXTURE_2D,texture4);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)(width/4), (int)(height/3),
    0,GL_RGB, GL_UNSIGNED_BYTE, data4);

    glGenTextures(1,&texture5);
    glBindTexture(GL_TEXTURE_2D,texture5);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)(width/4), (int)(height/3),
    0,GL_RGB, GL_UNSIGNED_BYTE, data5);

    glGenTextures(1,&texture6);
    glBindTexture(GL_TEXTURE_2D,texture6);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)(width/4), (int)(height/3),
    0,GL_RGB, GL_UNSIGNED_BYTE, data6);
    //*/
}

SkyboxFactory::~SkyboxFactory() = default;

void SkyboxFactory::draw() {
    int start = -size / 2;
    int bound = size / 2;
    int scale = 100;
    //	 11 16 20 o--------------o 07 12 19
    //			 /|			    /|
    //		    / |			   / |
    // 04 15 17 o--------------o 03 08 18
    //		   |  |			  |  |
    //	  10 13|22o-----------|--o 06 09 23
    //		   | /			  | /
    //         |/			  |/
    // 01 14 21 o--------------o 02 05 24
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture0);

    // glBindTexture(GL_TEXTURE_2D,texture1);
    glBegin(GL_QUADS);  // front
    glTexCoord2f(0, 1);
    glVertex3f(start * scale, start * scale, start * scale);  // 1
    glTexCoord2f(0, 0);
    glVertex3f(bound * scale, start * scale, start * scale);  // 2
    glTexCoord2f(1, 0);
    glVertex3f(bound * scale, bound * scale / 2, start * scale);  // 3
    glTexCoord2f(1, 1);
    glVertex3f(start * scale, bound * scale / 2, start * scale);  // 4
    glEnd();

    // glBindTexture(GL_TEXTURE_2D,texture2);
    glBegin(GL_QUADS);  // right
    glTexCoord2f(0, 1);
    glVertex3f(bound * scale, start * scale, start * scale);  // 5
    glTexCoord2f(0, 0);
    glVertex3f(bound * scale, start * scale, bound * scale);  // 6
    glTexCoord2f(1, 0);
    glVertex3f(bound * scale, bound * scale / 2, bound * scale);  // 7
    glTexCoord2f(1, 1);
    glVertex3f(bound * scale, bound * scale / 2, start * scale);  // 8
    glEnd();

    // glBindTexture(GL_TEXTURE_2D,texture3);
    glBegin(GL_QUADS);  // back
    glTexCoord2f(0, 1);
    glVertex3f(bound * scale, start * scale, bound * scale);  // 9
    glTexCoord2f(0, 0);
    glVertex3f(start * scale, start * scale, bound * scale);  // 10
    glTexCoord2f(1, 0);
    glVertex3f(start * scale, bound * scale / 2, bound * scale);  // 11
    glTexCoord2f(1, 1);
    glVertex3f(bound * scale, bound * scale / 2, bound * scale);  // 12
    glEnd();

    // glBindTexture(GL_TEXTURE_2D,texture4);
    glBegin(GL_QUADS);  // left
    glTexCoord2f(0, 1);
    glVertex3f(start * scale, start * scale, bound * scale);  // 13
    glTexCoord2f(0, 0);
    glVertex3f(start * scale, start * scale, start * scale);  // 14
    glTexCoord2f(1, 0);
    glVertex3f(start * scale, bound * scale / 2, start * scale);  // 15
    glTexCoord2f(1, 1);
    glVertex3f(start * scale, bound * scale / 2, bound * scale);  // 16
    glEnd();

    // glBindTexture(GL_TEXTURE_2D,texture5);
    glBegin(GL_QUADS);  // top
    glTexCoord2f(0, 1);
    glVertex3f(start * scale, bound * scale / 2, start * scale);  // 17
    glTexCoord2f(0, 0);
    glVertex3f(bound * scale, bound * scale / 2, start * scale);  // 18
    glTexCoord2f(1, 0);
    glVertex3f(bound * scale, bound * scale / 2, bound * scale);  // 19
    glTexCoord2f(1, 1);
    glVertex3f(start * scale, bound * scale / 2, bound * scale);  // 20
    glEnd();

    // glBindTexture(GL_TEXTURE_2D,texture6);
    glBegin(GL_QUADS);  // bottom
    glTexCoord2f(0, 1);
    glVertex3f(start * scale, start * scale, start * scale);  // 21
    glTexCoord2f(0, 0);
    glVertex3f(start * scale, start * scale, bound * scale);  // 22
    glTexCoord2f(1, 0);
    glVertex3f(bound * scale, start * scale, bound * scale);  // 23
    glTexCoord2f(1, 1);
    glVertex3f(bound * scale, start * scale, start * scale);  // 24
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void SkyboxFactory::printData() {
    printf("Checking Coordinates:\n");
    printf("(%d,%d)\n", image1_x_start, image1_y_start);
    printf("(%d,%d)\n", image1_x_end, image1_y_start);
    printf("(%d,%d)\n", image1_x_start, image1_y_end);
    printf("(%d,%d)\n", image1_x_end, image1_y_end);

    printf("(%d,%d)\n", image2_x_start, image2_y_start);
    printf("(%d,%d)\n", image2_x_end, image2_y_start);
    printf("(%d,%d)\n", image2_x_start, image2_y_end);
    printf("(%d,%d)\n", image2_x_end, image2_y_end);

    printf("(%d,%d)\n", image3_x_start, image3_y_start);
    printf("(%d,%d)\n", image3_x_end, image3_y_start);
    printf("(%d,%d)\n", image3_x_start, image3_y_end);
    printf("(%d,%d)\n", image3_x_end, image3_y_end);

    printf("(%d,%d)\n", image4_x_start, image4_y_start);
    printf("(%d,%d)\n", image4_x_end, image4_y_start);
    printf("(%d,%d)\n", image4_x_start, image4_y_end);
    printf("(%d,%d)\n", image4_x_end, image4_y_end);

    printf("(%d,%d)\n", image5_x_start, image5_y_start);
    printf("(%d,%d)\n", image5_x_end, image5_y_start);
    printf("(%d,%d)\n", image5_x_start, image5_y_end);
    printf("(%d,%d)\n", image5_x_end, image5_y_end);

    printf("(%d,%d)\n", image6_x_start, image6_y_start);
    printf("(%d,%d)\n", image6_x_end, image6_y_start);
    printf("(%d,%d)\n", image6_x_start, image6_y_end);
    printf("(%d,%d)\n", image6_x_end, image6_y_end);
}
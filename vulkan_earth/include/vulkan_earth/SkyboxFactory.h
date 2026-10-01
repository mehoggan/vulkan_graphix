#ifndef SKYBOX_FACTORY_H_
#define SKYBOX_FACTORY_H_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>

class SkyboxFactory {
public:
    SkyboxFactory();
    SkyboxFactory(int size_of_box);
    ~SkyboxFactory();
    void draw();
    void printData();

private:
    std::uint32_t texture0;
    std::uint32_t texture1;
    std::uint32_t texture2;
    std::uint32_t texture3;
    std::uint32_t texture4;
    std::uint32_t texture5;
    std::uint32_t texture6;
    float size;
    int image1_x_start;
    int image1_y_start;
    int image1_x_end;
    int image1_y_end;
    int image2_x_start;
    int image2_y_start;
    int image2_x_end;
    int image2_y_end;
    int image3_x_start;
    int image3_y_start;
    int image3_x_end;
    int image3_y_end;
    int image4_x_start;
    int image4_y_start;
    int image4_x_end;
    int image4_y_end;
    int image5_x_start;
    int image5_y_start;
    int image5_x_end;
    int image5_y_end;
    int image6_x_start;
    int image6_y_start;
    int image6_x_end;
    int image6_y_end;
};

#endif /* SKYBOX_FACTORY_H_ */
#ifndef CHASECAM_H
#define CHASECAM_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class ChaseCam {
public:
    ChaseCam();
    ChaseCam(float* new_target_pos, float* new_target_at);
    ~ChaseCam();
    void view();
    void setShakeCam(int magnitude);
    void updateShakeCam();
    void updateFactor();
    void resetFactor();

private:
    float* target_pos;
    float* target_at;
    int shake_cam_pos[3];
    float back_factor;
    float up_factor;
};

#endif
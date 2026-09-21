#ifndef CHASECAM_H
#define CHASECAM_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class ChaseCam {
public:
    ChaseCam();
    ChaseCam(GLfloat* new_target_pos, GLfloat* new_target_at);
    ~ChaseCam();
    void view();
    void setShakeCam(int magnitude);
    void updateShakeCam();
    void updateFactor();
    void resetFactor();

private:
    GLfloat* target_pos;
    GLfloat* target_at;
    int shake_cam_pos[3];
    GLfloat back_factor;
    GLfloat up_factor;
};

#endif
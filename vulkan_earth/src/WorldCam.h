#ifndef WORLDCAM_H
#define WORLDCAM_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class WorldCam {
public:
    WorldCam();
    WorldCam(GLfloat x, GLfloat y, GLfloat z);
    ~WorldCam();
    void view();
    void moveCam(float x, float y, float z);
    GLfloat* getMatrix();
    void setShakeCam(int magnitude);
    void updateShakeCam();

private:
    GLfloat matrix[16];
    int shake_cam_pos[3];
};

#endif
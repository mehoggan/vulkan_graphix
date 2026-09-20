#include "LoadingScreen.h"
#include "ImageObject.h"
#include "macro_crtdbg.h"

LoadingScreen::LoadingScreen() = default;
LoadingScreen::LoadingScreen(GLfloat x,
                             GLfloat y,
                             GLfloat z,
                             GLint width,
                             GLint height,
                             GLfloat red,
                             GLfloat green,
                             GLfloat blue,
                             GLfloat alpha) {
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
    this->width = width;
    this->height = height;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;
    image = new ImageObject(pos[0] + (width * 0.03),
                            pos[1] - (height * 0.03),
                            pos[2] + 2,
                            width * 0.94,
                            height * 0.94,
                            .006 * width,
                            1024,
                            1024,
                            "loading_screen.raw");
}
LoadingScreen::~LoadingScreen() { delete image; }

void LoadingScreen::draw() {
    glBegin(GL_QUADS);
    glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
    glVertex3f(pos[0], pos[1], pos[2]);
    glVertex3f(pos[0] - 3, pos[1] + 3, pos[2]);
    glVertex3f(pos[0] + width + 3, pos[1] + 3, pos[2]);
    glVertex3f(pos[0] + width, pos[1], pos[2]);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
    glVertex3f(pos[0] - 3, pos[1] + 3, pos[2]);
    glVertex3f(pos[0] - 3, pos[1] - height - 3, pos[2]);
    glVertex3f(pos[0], pos[1] - height, pos[2]);
    glVertex3f(pos[0], pos[1], pos[2]);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0], color[1], color[2], color[3]);
    glVertex3f(pos[0], pos[1], pos[2]);
    glVertex3f(pos[0], pos[1] - height, pos[2]);
    glVertex3f(pos[0] + width, pos[1] - height, pos[2]);
    glVertex3f(pos[0] + width, pos[1], pos[2]);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
    glVertex3f(pos[0] - 3, pos[1] - height - 3, pos[2]);
    glVertex3f(pos[0] + width + 3, pos[1] - height - 3, pos[2]);
    glVertex3f(pos[0] + width, pos[1] - height, pos[2]);
    glVertex3f(pos[0], pos[1] - height, pos[2]);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
    glVertex3f(pos[0] + width, pos[1], pos[2]);
    glVertex3f(pos[0] + width + 3, pos[1] + 3, pos[2]);
    glVertex3f(pos[0] + width + 3, pos[1] - height - 3, pos[2]);
    glVertex3f(pos[0] + width, pos[1] + -height, pos[2]);
    glEnd();

    image->draw();
}

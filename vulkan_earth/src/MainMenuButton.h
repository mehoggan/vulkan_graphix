#ifndef MAINMENUBUTTON_H
#define MAINMENUBUTTON_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include "Item.h"

class TextObject;
class SubMenu;

class MainMenuButton {
public:
    MainMenuButton();
    MainMenuButton(int id,
                   GLfloat new_x_pos,
                   GLfloat new_y_pos,
                   GLfloat new_z_pos,
                   GLfloat red,
                   GLfloat green,
                   GLfloat blue,
                   GLint new_width,
                   GLint new_height,
                   const std::string& new_caption,
                   SubMenu* new_submenu);
    ~MainMenuButton();
    void pressDraw();
    void draw();
    void pressButton();
    void activateSubMenu();
    void deactivateSubMenu();
    void depressButton();
    bool isPressed();
    bool isActive();
    void setLabel(const std::string& c);
    int getUNIQUEIDENTIFIER();
    GLfloat getXPos();
    GLfloat getYPos();
    GLfloat getHeight();
    GLfloat getWidth();
    GLfloat* getColor();
    void setColor(GLfloat r, GLfloat g, GLfloat b);
    SubMenu* getSubMenu();
    void printSelf(int i);

private:
    int uniqueidentifier;
    GLfloat x_pos;
    GLfloat y_pos;
    GLfloat z_pos;
    GLfloat color[4];
    GLint width;
    GLint height;
    std::string caption;
    TextObject* label;
    bool pressed;
    bool active;
    SubMenu* submenu;
};
#endif  // MAINMENUBUTTON_H
#ifndef MAINMENUBUTTON_H
#define MAINMENUBUTTON_H

#include "Item.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

class TextObject;
class SubMenu;

class MainMenuButton{
public:
	MainMenuButton();
	MainMenuButton(	int ID,
					GLfloat xPos, GLfloat yPos, GLfloat zPos, 
					GLfloat red, GLfloat green, GLfloat blue, 
					GLint width, GLint height, char* caption,
					SubMenu* submenu);
	~MainMenuButton();
	void pressDraw();
	void draw();
	void pressButton();
	void activateSubMenu();
	void deactivateSubMenu();
	void depressButton();
	bool isPressed();
	bool isActive();
	void setLabel(char* c);
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
	int UNIQUEIDENTIFIER;
	GLfloat xPos;
	GLfloat yPos;
	GLfloat zPos;
	GLfloat color[4];
	GLint width;
	GLint height;
	char* caption;
	TextObject* label;
	bool pressed;
	bool active;
	SubMenu* submenu;
};
#endif //MAINMENUBUTTON_H
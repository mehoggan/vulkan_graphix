#include <GL/glew.h>
#include <GL/freeglut.h>

#ifndef Control_ITEM_BUTTON_H
#define Control_ITEM_BUTTON_H

class TextObject;
class SubMenu;
class SubMenuLandscape;

class ControlItemButton : public ControlItem{
public:
	ControlItemButton();
	ControlItemButton(	SubMenuLandscape* parent,
						GLfloat xPos,GLfloat yPos,GLfloat zPos,
						GLfloat red,GLfloat green,GLfloat blue,
						GLint width,GLint height,char* caption);
	~ControlItemButton();
	void draw();
	void mouseClickEvent(GLint x, GLint y, GLint state, bool stillOverControlItemCheckBox);
	GLfloat getXPos();
	GLfloat getYPos();
	GLfloat getHeight();
	GLfloat getWidth();
	char* collectData();
	void updateMouse(int x, int y);
	void setOptionText(int index);
	void setOptionText(char* newText);
	void updateButtonState();
	bool isToggled();
	void setToggled(bool t);

private:
	GLfloat xPos;
	GLfloat yPos;
	GLfloat zPos;
	GLfloat color[4];
	GLint width;
	GLint height;
	TextObject* label;
	char* caption;
	int menuState;
	int buttonState; //0 = no button pressed, 1 = up button pressed, 2 = down button pressed
	SubMenuLandscape* parent;
	bool toggled;
};
#endif //Control_ITEM_BUTTON_H
#ifndef Control_ITEM_Check_BOX_H
#define Control_ITEM_Check_BOX_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class TextObject;
class ControlItem;
                                     
class ControlItemCheckBox : public ControlItem {
public:
	ControlItemCheckBox();
	ControlItemCheckBox(	GLfloat xPos, GLfloat yPos, GLfloat zPos, 
								GLfloat red, GLfloat green, GLfloat blue, 
								GLint width, GLint height, char* caption);
	~ControlItemCheckBox();
	void draw();
	void mouseClickEvent(GLint x, GLint y, GLint state, bool stillOverControlItemCheckBox);
	GLfloat getXPos();
	GLfloat getYPos();
	GLfloat getHeight();
	GLfloat getWidth();
	char* collectData();
	void updateMouse(int x, int y);

	
private:
	void setOptionText(int index);
	void setOptionText(char* newText);
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
};
#endif //Control_ITEM_Check_BOX_H
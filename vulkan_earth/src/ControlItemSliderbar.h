#ifndef Control_ITEM_SLIDERBAR_H
#define Control_ITEM_SLIDERBAR_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class ControlItem;
class TextObject;

class ControlItemSliderbar : public ControlItem {
public:
	ControlItemSliderbar();
	ControlItemSliderbar(GLfloat xPos, GLfloat yPos, GLfloat zPos, 
								GLfloat red, GLfloat green, GLfloat blue, 
								GLint width, GLint height, char* caption,
								char* menuInfo, int sliderStartingPos);
	~ControlItemSliderbar();
	void draw();
	void mouseClickEvent(GLint x, GLint y, GLint state, bool stillOverControlItemSliderbar);
	GLfloat getXPos();
	GLfloat getYPos();
	GLfloat getHeight();
	GLfloat getWidth();
	GLfloat getBarXPos();
	GLfloat getSliderXPos();
	void setSliderXPos(GLfloat x);
	GLfloat getInterval();
	char* collectData();
	void updateMouse(int x, int y);

private:
	void setOptionText(int index);
	void setOptionText(char* newText);
	GLfloat xPos;
	GLfloat yPos;
	GLfloat zPos;
	GLfloat barXPos;
	GLfloat barYPos;
	GLfloat barZPos;
	GLint barWidth;
	GLfloat sliderXPos;
	GLfloat sliderYPos;
	GLfloat sliderZPos;
	GLint sliderWidth;
	GLint sliderHeight;
	GLfloat color[4];
	GLint width;
	GLint height;
	GLfloat interval;
	TextObject* optionText;
	TextObject* label;
	char* caption;
	char* menuInfo;
	char* currentOption;
	int menuState;
	int buttonState;	//0 = no button pressed, 1 = up button pressed, 2 = down button pressed
	int numberOfOptions;
	char** allOptions;
	bool isSliderClicked;
};

#endif
#ifndef SUB_MENU_HARDWARE_H
#define SUB_MENU_HARDWARE_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class TextObject;
class SubMenu;
class ControlItem;
class ControlItemCheckBox;
class ControlItemSelectionBox;

const int NUM_CONTROL_ITEMS_HW = 1;

class SubMenuHardware : public SubMenu{
	public:
		SubMenuHardware();
		SubMenuHardware(int ID,
					GLfloat xPos, GLfloat yPos, GLfloat zPos, 
					GLfloat red, GLfloat green, GLfloat blue, 
					GLint width, GLint height, char* caption,
					GLfloat percentBorder);
		~SubMenuHardware();
		int getUNIQUEIDENTIFIER();
		void setUNIQUEIDENTIFIER(int ID);
		GLfloat getXPos();
		void setXPos(GLfloat newXpos);
		GLfloat getYPos();
		void setYPos(GLfloat newYpos);
		GLfloat getZPos();
		void setZPos(GLfloat newZpos);
		GLfloat getRed();
		void setRed(GLfloat red);
		GLfloat getGreen();
		void setGreen(GLfloat green);
		GLfloat getBlue();
		void setBlue(GLfloat blue);
		GLint getWidth(); 
		void setWdith(GLint width);
		GLint getHeight();
		void setHeight(GLint height); 
		char* getCaption();
		void setCaption(char* caption);
		GLfloat getPerecentBorder();
		void setPercentBorder(GLfloat percent);
		void draw();
		const char* collectData();
		void subMenuMouseTest(int x, int y, int buttonDown);
		void updateMouse(int x, int y);
	private:
		int UNIQUEIDENTIFIER;
		GLfloat xPos;
		GLfloat yPos;
		GLfloat zPos;
		GLfloat color[4];
		GLint width;
		GLint height;
		char* caption;
		GLfloat percentBorder;
		TextObject* label;
		ControlItem* subMenuButton[NUM_CONTROL_ITEMS_HW];
		ControlItem* buttonPressed;
};

#endif	//	SUB_MENU_HARDWARE_H
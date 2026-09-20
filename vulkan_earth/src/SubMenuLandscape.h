#ifndef SUB_MENU_LANDSCAPE_H
#define SUB_MENU_LANDSCAPE_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class TextObject;
class SubMenu;
class ControlItem;
class ControlItemCheckBox;
class ControlItemSelectionBox;
class ControlItemSliderbar;
class TerrainMaker;

const int NUM_CONTROL_ITEMS_LND = 4;
const int PREVIEW_BUTTON = 3;

class SubMenuLandscape : public SubMenu{
	public:
		SubMenuLandscape();
		SubMenuLandscape(int ID,
					GLfloat xPos, GLfloat yPos, GLfloat zPos, 
					GLfloat red, GLfloat green, GLfloat blue, 
					GLint width, GLint height, char* caption,
					GLfloat percentBorder);
		~SubMenuLandscape();
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
		TerrainMaker* tm;										//PUBLIC BECAUSE I AM TOO LAZY TO UPDATE ENTIRE INTERFACE FOR ONE GET FUNCTION
		ControlItem* subMenuButton[NUM_CONTROL_ITEMS_LND];		//BOTH THESE ITEMS NEED GETTERS AND SETTERS WHICH MEANS UPDATE TO INTERFACE
	private:
		int UNIQUEIDENTIFIER;
		int oldMouseX,oldMouseY;
		GLfloat camX,camY,camZ;
		GLfloat xPos;
		GLfloat yPos;
		GLfloat zPos;
		GLfloat color[4];
		GLint width;
		GLint height;
		char* caption;
		GLfloat percentBorder;
		TextObject* label;
		ControlItem* buttonPressed;
		int numberpressed;
};

#endif	//	SUB_MENU_LANDSCAPE_H
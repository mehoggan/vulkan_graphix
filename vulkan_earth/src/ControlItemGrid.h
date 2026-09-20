#ifndef CONTROL_ITEM_GRID_H
#define CONTROL_ITEM_GRID_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class ControlItemButton;
class ImageObject;
class TextObject;

class ControlItemGrid : public ControlItem{
public:
	ControlItemGrid();
	ControlItemGrid(GLfloat xPos, GLfloat yPos, GLfloat zPos, GLint width, GLint height, int rows, int cols,
							GLfloat activeCellColorRed, GLfloat activeCellColorGreen, GLfloat activeCellColorBlue, bool visibleLines, bool multiSelectable);
	~ControlItemGrid();
	void draw();
	void mouseClickEvent(GLint x, GLint y, GLint state, bool stillOverArrowButton);
	void updateMouse(int x, int y);
	GLfloat getXPos();
	GLfloat getYPos();
	GLfloat getHeight();
	GLfloat getWidth();
	char* collectData();
	void setOptionText(int index);
	void setOptionText(char* newText);
	void setImageSizeToCell(ImageObject* img, float scale);
	void placeImageToCell(ImageObject* img, int row, int col);
	void placeTextToCell(TextObject* text, int row, int col);
	void deselectAllCells();
	bool* getSelectedCells();
	void selectCell(int row, int col);

private:
	GLfloat xPos, yPos, zPos;
	GLfloat activeCellColor[4];
	GLint width, height;
	GLfloat cellWidth, cellHeight;
	int rows, cols;
	bool visibleLines;
	bool multiSelectable;
	bool* selectedCells;
	ControlItemButton** buttons;
	
};

#endif
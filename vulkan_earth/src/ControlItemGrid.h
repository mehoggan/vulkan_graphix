#ifndef CONTROL_ITEM_GRID_H
#define CONTROL_ITEM_GRID_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>

#include "ControlItem.h"

class ControlItemButton;
class ImageObject;
class TextObject;

class ControlItemGrid : public ControlItem {
public:
    ControlItemGrid();
    ControlItemGrid(GLfloat x_pos,
                    GLfloat y_pos,
                    GLfloat z_pos,
                    GLint width,
                    GLint height,
                    int rows,
                    int cols,
                    GLfloat active_cell_color_red,
                    GLfloat active_cell_color_green,
                    GLfloat active_cell_color_blue,
                    bool visible_lines,
                    bool multi_selectable);
    ~ControlItemGrid() override;
    void draw() override;
    void mouseClickEvent(GLint x,
                         GLint y,
                         GLint state,
                         bool still_over_arrow_button) override;
    void updateMouse(int x, int y) override;
    GLfloat getXPos() override;
    GLfloat getYPos() override;
    GLfloat getHeight() override;
    GLfloat getWidth() override;
    std::string collectData() override;
    void setOptionText(int index) override;
    void setOptionText(const std::string& new_text) override;
    void setImageSizeToCell(ImageObject* img, float scale);
    void placeImageToCell(ImageObject* img, int row, int col);
    void placeTextToCell(TextObject* text, int row, int col);
    void deselectAllCells();
    bool* getSelectedCells();
    void selectCell(int row, int col);

private:
    GLfloat x_pos, y_pos, z_pos;
    GLfloat active_cell_color[4];
    GLint width, height;
    GLfloat cell_width, cell_height;
    int rows, cols;
    bool visible_lines;
    bool multi_selectable;
    bool* selected_cells;
    ControlItemButton** buttons;
};

#endif
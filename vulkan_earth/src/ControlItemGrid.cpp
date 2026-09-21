#include "ControlItemGrid.h"
#include "ControlItem.h"
#include "ControlItemButton.h"
#include "ImageObject.h"
#include "TextObject.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ControlItemGrid::ControlItemGrid() = default;
ControlItemGrid::ControlItemGrid(GLfloat new_x_pos,
                                 GLfloat new_y_pos,
                                 GLfloat new_z_pos,
                                 GLint new_width,
                                 GLint new_height,
                                 int new_rows,
                                 int new_cols,
                                 GLfloat active_cell_color_red,
                                 GLfloat active_cell_color_green,
                                 GLfloat active_cell_color_blue,
                                 bool new_visible_lines,
                                 bool new_multi_selectable) {
    this->x_pos = new_x_pos;
    this->y_pos = new_y_pos;
    this->z_pos = new_z_pos;
    this->width = new_width;
    this->height = new_height;
    this->rows = new_rows;
    this->cols = new_cols;
    cell_width = new_width / (new_cols * 1.0);
    cell_height = new_height / (new_rows * 1.0);
    active_cell_color[0] = active_cell_color_red;
    active_cell_color[1] = active_cell_color_green;
    active_cell_color[2] = active_cell_color_blue;
    active_cell_color[3] = 1;
    this->visible_lines = new_visible_lines;
    this->multi_selectable = new_multi_selectable;

    selected_cells = new bool[new_rows * new_cols];
    buttons = new ControlItemButton*[new_rows * new_cols];

    for (int i = 0; i < new_rows * new_cols; i++) selected_cells[i] = false;

    // create buttons and place them in the grid in the order:(0,0), (0,1),
    // (0,2), .....
    int button_i = 0;
    for (int r = 0; r < new_rows; r++) {
        for (int c = 0; c < new_cols; c++) {
            buttons[button_i] =
                    new ControlItemButton(nullptr,
                                          this->x_pos + cell_width * c,
                                          this->y_pos - cell_height * r,
                                          this->z_pos + 0.5,
                                          0.7,
                                          0.7,
                                          0.7,
                                          cell_width,
                                          cell_height,
                                          "ABC");
            button_i++;
        }
    }
}

ControlItemGrid::~ControlItemGrid() {
    for (int i = 0; i < rows * cols; i++) {
        delete buttons[i];
    }
    delete[] buttons;
    delete[] selected_cells;
}

void ControlItemGrid::draw() {
    glPushMatrix();

    // Draw main body
    glBegin(GL_QUADS);
    glColor4f(0.75 - 0.2f, 0.75 - 0.2f, 0.75 - 0.2f, 1);
    glVertex3f(this->x_pos, this->y_pos, this->z_pos + 0.4);
    glVertex3f(this->x_pos - 4, this->y_pos + 4, this->z_pos + 0.4);
    glVertex3f(this->x_pos + width + 4, this->y_pos + 4, this->z_pos + 0.4);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos + 0.4);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(0.75 - 0.2f, 0.75 - 0.2f, 0.75 - 0.2f, 1);
    glVertex3f(this->x_pos - 4, this->y_pos + 4, this->z_pos + 0.4);
    glVertex3f(this->x_pos - 4, this->y_pos - height - 4, this->z_pos + 0.4);
    glVertex3f(this->x_pos, this->y_pos - height, this->z_pos + 0.4);
    glVertex3f(this->x_pos, this->y_pos, this->z_pos + 0.4);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(0.45, 0.45, 0.45, 1);
    glVertex3f(this->x_pos, this->y_pos, this->z_pos + 0.4);
    glVertex3f(this->x_pos, this->y_pos - height, this->z_pos + 0.4);
    glVertex3f(this->x_pos + width, this->y_pos - height, this->z_pos + 0.4);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos + 0.4);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(0.75 + 0.4f, 0.75 + 0.4f, 0.75 + 0.4f, 1);
    glVertex3f(this->x_pos - 4, this->y_pos - height - 4, this->z_pos + 0.4);
    glVertex3f(this->x_pos + width + 4,
               this->y_pos - height - 4,
               this->z_pos + 0.4);
    glVertex3f(this->x_pos + width, this->y_pos - height, this->z_pos + 0.4);
    glVertex3f(this->x_pos, this->y_pos - height, this->z_pos + 0.4);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(0.75 + 0.4f, 0.75 + 0.4f, 0.75 + 0.4f, 1);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos + 0.4);
    glVertex3f(this->x_pos + width + 4, this->y_pos + 4, this->z_pos + 0.4);
    glVertex3f(this->x_pos + width + 4,
               this->y_pos - height - 4,
               this->z_pos + 0.4);
    glVertex3f(this->x_pos + width, this->y_pos + -height, this->z_pos + 0.4);
    glEnd();

    // Draw cell lines if they are set to visible
    if (visible_lines) {
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                glBegin(GL_LINE_LOOP);
                glColor3f(0, 0, 0);
                glVertex3f(this->x_pos + cell_width * c,
                           this->y_pos - cell_height * r,
                           this->z_pos + 0.5);
                glVertex3f(this->x_pos + cell_width * c,
                           this->y_pos - cell_height * (r + 1),
                           this->z_pos + 0.5);
                glVertex3f(this->x_pos + cell_width * (c + 1),
                           this->y_pos - cell_height * (r + 1),
                           this->z_pos + 0.5);
                glVertex3f(this->x_pos + cell_width * (c + 1),
                           this->y_pos - cell_height * r,
                           this->z_pos + 0.5);
                glEnd();
            }
    }
    // Check button placements
    /*
    for(int i=0 ; i<rows*cols ; i++){
        buttons[i]->draw();
    }
    //*/

    // Change the color of active cells
    for (int i = 0; i < rows * cols; i++) {
        if (buttons[i]->isToggled()) {
            glBegin(GL_QUADS);
            glColor3f(active_cell_color[0],
                      active_cell_color[1],
                      active_cell_color[2]);
            glVertex3f(buttons[i]->getXPos() + 3,
                       buttons[i]->getYPos() - 3,
                       this->z_pos + 0.6);
            glVertex3f(buttons[i]->getXPos() + 3,
                       buttons[i]->getYPos() - buttons[i]->getHeight() + 3,
                       this->z_pos + 0.6);
            glVertex3f(buttons[i]->getXPos() + buttons[i]->getWidth() - 3,
                       buttons[i]->getYPos() - buttons[i]->getHeight() + 3,
                       this->z_pos + 0.6);
            glVertex3f(buttons[i]->getXPos() + buttons[i]->getWidth() - 3,
                       buttons[i]->getYPos() - 3,
                       this->z_pos + 0.6);
            glEnd();
        }
    }
    glPopMatrix();
}

void ControlItemGrid::mouseClickEvent(GLint x,
                                      GLint y,
                                      GLint state,
                                      bool still_over_arrow_button) {
    if (state == 1) {
        // If THE CLICK OCUURED INSIDE OF THE GRID
        if ((x_pos <= x && x <= x_pos + width) &&
            (y_pos - height <= y && y <= y_pos)) {
            playSFX(ITEM_SELECTED);

            // IF THE GRID DOES NOT ALLOW MULTIPLE SELECTION
            if (!multi_selectable) {
                // first untoggle all the buttons
                deselectAllCells();
                // find the button that's been clicked on and update the state
                // of the button
                for (int i = 0; i < rows * cols; i++) {
                    buttons[i]->mouseClickEvent(x, y, state, true);
                    if (buttons[i]->isToggled()) {
                        selected_cells[i] = true;
                        buttons[i]->updateButtonState();
                        break;
                    }
                }
            }
            // IF THE GRID ALLOWS MULTIPLE SELECTION
            else {
                for (int i = 0; i < rows * cols; i++) {
                    if ((x >= (buttons[i]->getXPos()) &&
                         x <= ((buttons[i]->getXPos()) +
                               (buttons[i]->getWidth()))) &&
                        (y <= (buttons[i]->getYPos()) &&
                         y >= ((buttons[i]->getYPos()) -
                               (buttons[i]->getHeight())))) {
                        buttons[i]->mouseClickEvent(x, y, state, true);
                        if (buttons[i]->isToggled() && !selected_cells[i]) {
                            selected_cells[i] = true;
                            buttons[i]->updateButtonState();
                            break;
                        } else if (buttons[i]->isToggled() &&
                                   selected_cells[i]) {
                            buttons[i]->setToggled(false);
                            selected_cells[i] = false;
                            buttons[i]->updateButtonState();
                            break;
                        }
                    }
                }
            }
        }
    }
}

// GETTERS & SETTERS
GLfloat ControlItemGrid::getXPos() { return this->x_pos; }
GLfloat ControlItemGrid::getYPos() { return this->y_pos; }
GLfloat ControlItemGrid::getHeight() { return this->height; }
GLfloat ControlItemGrid::getWidth() { return this->width; }
bool* ControlItemGrid::getSelectedCells() { return &selected_cells[0]; }

void ControlItemGrid::selectCell(int row_index, int col_index) {
    // Check for valid indexing
    if (0 <= row_index && row_index < rows && 0 <= col_index &&
        col_index < cols) {
        if (!multi_selectable) deselectAllCells();
        buttons[cols * row_index + col_index]->setToggled(true);
        buttons[cols * row_index + col_index]->updateButtonState();
        selected_cells[cols * row_index + col_index] = true;
        playSFX(ITEM_SELECTED);
    } else {
        printf("ERROR <ControlItemGrid::selectCell(int, int)>: Wrong "
               "indexing.\n");
    }
}

void ControlItemGrid::deselectAllCells() {
    for (int i = 0; i < rows * cols; i++) {
        buttons[i]->setToggled(false);
        buttons[i]->updateButtonState();
        selected_cells[i] = false;
    }
}

void ControlItemGrid::setImageSizeToCell(ImageObject* img, float scale) {
    img->setWidth(cell_width * scale);
    img->setHeight(cell_height * scale);
}

void ControlItemGrid::placeImageToCell(ImageObject* img, int row, int col) {
    img->setXpos(this->x_pos + (cell_width * col) +
                 ((cell_width - img->getWidth()) / 2.0));
    img->setYpos(this->y_pos - (cell_height * row) -
                 ((cell_height - img->getHeight()) / 2.0));
    img->setZpos(this->z_pos + 1);
}

void ControlItemGrid::placeTextToCell(TextObject* text, int row, int col) {
    text->setXpos(this->x_pos + (cell_width * col) + (cell_width * 0.1));
    text->setYpos(this->y_pos - (cell_height * row) - (cell_height * 0.85));
    text->setZpos(this->z_pos + 2);
}

// DUMMY FUNCTIONS
void ControlItemGrid::setOptionText(int index) {}
void ControlItemGrid::setOptionText(const std::string& new_text) {}
void ControlItemGrid::updateMouse(int x, int y) {}
std::string ControlItemGrid::collectData() { return ""; }

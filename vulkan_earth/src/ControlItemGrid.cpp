#include "ControlItemGrid.h"
#include "ControlItem.h"
#include "ControlItemButton.h"
#include "ImageObject.h"
#include "TextObject.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ControlItemGrid::ControlItemGrid() = default;
ControlItemGrid::ControlItemGrid(GLfloat xPos,
                                 GLfloat yPos,
                                 GLfloat zPos,
                                 GLint width,
                                 GLint height,
                                 int rows,
                                 int cols,
                                 GLfloat activeCellColorRed,
                                 GLfloat activeCellColorGreen,
                                 GLfloat activeCellColorBlue,
                                 bool visibleLines,
                                 bool multiSelectable) {
    this->xPos = xPos;
    this->yPos = yPos;
    this->zPos = zPos;
    this->width = width;
    this->height = height;
    this->rows = rows;
    this->cols = cols;
    cellWidth = width / (cols * 1.0);
    cellHeight = height / (rows * 1.0);
    activeCellColor[0] = activeCellColorRed;
    activeCellColor[1] = activeCellColorGreen;
    activeCellColor[2] = activeCellColorBlue;
    activeCellColor[3] = 1;
    this->visibleLines = visibleLines;
    this->multiSelectable = multiSelectable;

    selectedCells = new bool[rows * cols];
    buttons = new ControlItemButton*[rows * cols];

    for (int i = 0; i < rows * cols; i++) selectedCells[i] = false;

    // create buttons and place them in the grid in the order:(0,0), (0,1),
    // (0,2), .....
    int button_i = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            buttons[button_i] =
                    new ControlItemButton(nullptr,
                                          this->xPos + cellWidth * c,
                                          this->yPos - cellHeight * r,
                                          this->zPos + 0.5,
                                          0.7,
                                          0.7,
                                          0.7,
                                          cellWidth,
                                          cellHeight,
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
    delete[] selectedCells;
}

void ControlItemGrid::draw() {
    glPushMatrix();

    // Draw main body
    glBegin(GL_QUADS);
    glColor4f(0.75 - 0.2f, 0.75 - 0.2f, 0.75 - 0.2f, 1);
    glVertex3f(this->xPos, this->yPos, this->zPos + 0.4);
    glVertex3f(this->xPos - 4, this->yPos + 4, this->zPos + 0.4);
    glVertex3f(this->xPos + width + 4, this->yPos + 4, this->zPos + 0.4);
    glVertex3f(this->xPos + width, this->yPos, this->zPos + 0.4);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(0.75 - 0.2f, 0.75 - 0.2f, 0.75 - 0.2f, 1);
    glVertex3f(this->xPos - 4, this->yPos + 4, this->zPos + 0.4);
    glVertex3f(this->xPos - 4, this->yPos - height - 4, this->zPos + 0.4);
    glVertex3f(this->xPos, this->yPos - height, this->zPos + 0.4);
    glVertex3f(this->xPos, this->yPos, this->zPos + 0.4);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(0.45, 0.45, 0.45, 1);
    glVertex3f(this->xPos, this->yPos, this->zPos + 0.4);
    glVertex3f(this->xPos, this->yPos - height, this->zPos + 0.4);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos + 0.4);
    glVertex3f(this->xPos + width, this->yPos, this->zPos + 0.4);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(0.75 + 0.4f, 0.75 + 0.4f, 0.75 + 0.4f, 1);
    glVertex3f(this->xPos - 4, this->yPos - height - 4, this->zPos + 0.4);
    glVertex3f(
            this->xPos + width + 4, this->yPos - height - 4, this->zPos + 0.4);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos + 0.4);
    glVertex3f(this->xPos, this->yPos - height, this->zPos + 0.4);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(0.75 + 0.4f, 0.75 + 0.4f, 0.75 + 0.4f, 1);
    glVertex3f(this->xPos + width, this->yPos, this->zPos + 0.4);
    glVertex3f(this->xPos + width + 4, this->yPos + 4, this->zPos + 0.4);
    glVertex3f(
            this->xPos + width + 4, this->yPos - height - 4, this->zPos + 0.4);
    glVertex3f(this->xPos + width, this->yPos + -height, this->zPos + 0.4);
    glEnd();

    // Draw cell lines if they are set to visible
    if (visibleLines) {
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                glBegin(GL_LINE_LOOP);
                glColor3f(0, 0, 0);
                glVertex3f(this->xPos + cellWidth * c,
                           this->yPos - cellHeight * r,
                           this->zPos + 0.5);
                glVertex3f(this->xPos + cellWidth * c,
                           this->yPos - cellHeight * (r + 1),
                           this->zPos + 0.5);
                glVertex3f(this->xPos + cellWidth * (c + 1),
                           this->yPos - cellHeight * (r + 1),
                           this->zPos + 0.5);
                glVertex3f(this->xPos + cellWidth * (c + 1),
                           this->yPos - cellHeight * r,
                           this->zPos + 0.5);
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
            glColor3f(activeCellColor[0],
                      activeCellColor[1],
                      activeCellColor[2]);
            glVertex3f(buttons[i]->getXPos() + 3,
                       buttons[i]->getYPos() - 3,
                       this->zPos + 0.6);
            glVertex3f(buttons[i]->getXPos() + 3,
                       buttons[i]->getYPos() - buttons[i]->getHeight() + 3,
                       this->zPos + 0.6);
            glVertex3f(buttons[i]->getXPos() + buttons[i]->getWidth() - 3,
                       buttons[i]->getYPos() - buttons[i]->getHeight() + 3,
                       this->zPos + 0.6);
            glVertex3f(buttons[i]->getXPos() + buttons[i]->getWidth() - 3,
                       buttons[i]->getYPos() - 3,
                       this->zPos + 0.6);
            glEnd();
        }
    }
    glPopMatrix();
}

void ControlItemGrid::mouseClickEvent(GLint x,
                                      GLint y,
                                      GLint state,
                                      bool stillOverArrowButton) {
    if (state == 1) {
        // If THE CLICK OCUURED INSIDE OF THE GRID
        if ((xPos <= x && x <= xPos + width) &&
            (yPos - height <= y && y <= yPos)) {
            playSFX(ITEM_SELECTED);

            // IF THE GRID DOES NOT ALLOW MULTIPLE SELECTION
            if (!multiSelectable) {
                // first untoggle all the buttons
                deselectAllCells();
                // find the button that's been clicked on and update the state
                // of the button
                for (int i = 0; i < rows * cols; i++) {
                    buttons[i]->mouseClickEvent(x, y, state, true);
                    if (buttons[i]->isToggled()) {
                        selectedCells[i] = true;
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
                        if (buttons[i]->isToggled() && !selectedCells[i]) {
                            selectedCells[i] = true;
                            buttons[i]->updateButtonState();
                            break;
                        } else if (buttons[i]->isToggled() &&
                                   selectedCells[i]) {
                            buttons[i]->setToggled(false);
                            selectedCells[i] = false;
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
GLfloat ControlItemGrid::getXPos() { return this->xPos; }
GLfloat ControlItemGrid::getYPos() { return this->yPos; }
GLfloat ControlItemGrid::getHeight() { return this->height; }
GLfloat ControlItemGrid::getWidth() { return this->width; }
bool* ControlItemGrid::getSelectedCells() { return &selectedCells[0]; }

void ControlItemGrid::selectCell(int row_index, int col_index) {
    // Check for valid indexing
    if (0 <= row_index && row_index < rows && 0 <= col_index &&
        col_index < cols) {
        if (!multiSelectable) deselectAllCells();
        buttons[cols * row_index + col_index]->setToggled(true);
        buttons[cols * row_index + col_index]->updateButtonState();
        selectedCells[cols * row_index + col_index] = true;
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
        selectedCells[i] = false;
    }
}

void ControlItemGrid::setImageSizeToCell(ImageObject* img, float scale) {
    img->setWidth(cellWidth * scale);
    img->setHeight(cellHeight * scale);
}

void ControlItemGrid::placeImageToCell(ImageObject* img, int row, int col) {
    img->setXpos(this->xPos + (cellWidth * col) +
                 ((cellWidth - img->getWidth()) / 2.0));
    img->setYpos(this->yPos - (cellHeight * row) -
                 ((cellHeight - img->getHeight()) / 2.0));
    img->setZpos(this->zPos + 1);
}

void ControlItemGrid::placeTextToCell(TextObject* text, int row, int col) {
    text->setXpos(this->xPos + (cellWidth * col) + (cellWidth * 0.1));
    text->setYpos(this->yPos - (cellHeight * row) - (cellHeight * 0.85));
    text->setZpos(this->zPos + 2);
}

// DUMMY FUNCTIONS
void ControlItemGrid::setOptionText(int index) {}
void ControlItemGrid::setOptionText(const std::string& newText) {}
void ControlItemGrid::updateMouse(int x, int y) {}
std::string ControlItemGrid::collectData() { return ""; }

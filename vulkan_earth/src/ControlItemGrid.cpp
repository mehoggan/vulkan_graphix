#include "vulkan_earth/ControlItemGrid.h"
#include <array>
#include <cstdint>
#include <vector>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/ControlItemButton.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/ImageObject.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/MacroCrtdbg.h"

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

extern void playSFX(std::int32_t sfx);

ControlItemGrid::ControlItemGrid() = default;
ControlItemGrid::ControlItemGrid(float new_x_pos,
                                 float new_y_pos,
                                 float new_z_pos,
                                 std::int32_t new_width,
                                 std::int32_t new_height,
                                 std::int32_t new_rows,
                                 std::int32_t new_cols,
                                 float active_cell_color_red,
                                 float active_cell_color_green,
                                 float active_cell_color_blue,
                                 bool new_visible_lines,
                                 bool new_multi_selectable) {
    x_pos = new_x_pos;
    y_pos = new_y_pos;
    z_pos = new_z_pos;
    width = new_width;
    height = new_height;
    rows = new_rows;
    cols = new_cols;
    cell_width = new_width / (new_cols * 1.0);
    cell_height = new_height / (new_rows * 1.0);
    active_cell_color[0] = active_cell_color_red;
    active_cell_color[1] = active_cell_color_green;
    active_cell_color[2] = active_cell_color_blue;
    active_cell_color[3] = 1;
    visible_lines = new_visible_lines;
    multi_selectable = new_multi_selectable;

    selected_cells = new bool[new_rows * new_cols];
    buttons = new ControlItemButton*[new_rows * new_cols];

    for (std::int32_t i = 0; i < new_rows * new_cols; i++)
        selected_cells[i] = false;

    // create buttons and place them in the grid in the order:(0,0), (0,1),
    // (0,2), .....
    std::int32_t button_i = 0;
    for (std::int32_t r = 0; r < new_rows; r++) {
        for (std::int32_t c = 0; c < new_cols; c++) {
            buttons[button_i] = new ControlItemButton(nullptr,
                                                      x_pos + cell_width * c,
                                                      y_pos - cell_height * r,
                                                      z_pos + 0.5,
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
    for (std::int32_t i = 0; i < rows * cols; i++) {
        delete buttons[i];
    }
    delete[] buttons;
    delete[] selected_cells;
}

void ControlItemGrid::draw(render::RenderContext& context) {
    using Vec3 = math::Vec3<float>;
    using Vec4 = math::Vec4<float>;
    std::vector<bool> toggled(rows * cols);
    for (std::int32_t i = 0; i < rows * cols; i++) {
        toggled[i] = buttons[i]->isToggled();
    }
    if (mesh_built && toggled == built_toggled) {
        context.draw(mesh);
        return;
    }
    mesh.clear();

    // Draw main body (sunken bevel: -0.2 top/left, +0.4 bottom/right)
    vulkan_earth::appendFrame(mesh,
                              x_pos,
                              y_pos,
                              z_pos + 0.4,
                              width,
                              height,
                              Vec4(0.75 - 0.2f, 0.75 - 0.2f, 0.75 - 0.2f, 1),
                              Vec4(0.45, 0.45, 0.45, 1),
                              Vec4(0.75 + 0.4f, 0.75 + 0.4f, 0.75 + 0.4f, 1),
                              4);

    // Draw cell lines if they are set to visible
    if (visible_lines) {
        Vec4 const black(0, 0, 0, 1);
        for (std::int32_t r = 0; r < rows; r++)
            for (std::int32_t c = 0; c < cols; c++) {
                // GL_LINE_LOOP: the four edges, closing back to the start.
                std::array<Vec3, 4> const loop = {
                        Vec3(x_pos + cell_width * c,
                             y_pos - cell_height * r,
                             z_pos + 0.5),
                        Vec3(x_pos + cell_width * c,
                             y_pos - cell_height * (r + 1),
                             z_pos + 0.5),
                        Vec3(x_pos + cell_width * (c + 1),
                             y_pos - cell_height * (r + 1),
                             z_pos + 0.5),
                        Vec3(x_pos + cell_width * (c + 1),
                             y_pos - cell_height * r,
                             z_pos + 0.5)};
                for (std::size_t corner = 0; corner < loop.size(); ++corner) {
                    mesh.addLine(loop[corner],
                                 loop[(corner + 1) % loop.size()],
                                 black);
                }
            }
    }
    // Check button placements
    /*
    for(int i=0 ; i<rows*cols ; i++){
        buttons[i]->draw();
    }
    //*/

    // Change the color of active cells
    for (std::int32_t i = 0; i < rows * cols; i++) {
        if (toggled[i]) {
            mesh.addQuad(
                    {Vec3(buttons[i]->getXPos() + 3,
                          buttons[i]->getYPos() - 3,
                          z_pos + 0.6),
                     Vec3(buttons[i]->getXPos() + 3,
                          buttons[i]->getYPos() - buttons[i]->getHeight() + 3,
                          z_pos + 0.6),
                     Vec3(buttons[i]->getXPos() + buttons[i]->getWidth() - 3,
                          buttons[i]->getYPos() - buttons[i]->getHeight() + 3,
                          z_pos + 0.6),
                     Vec3(buttons[i]->getXPos() + buttons[i]->getWidth() - 3,
                          buttons[i]->getYPos() - 3,
                          z_pos + 0.6)},
                    Vec4(active_cell_color[0],
                         active_cell_color[1],
                         active_cell_color[2],
                         1));
        }
    }
    built_toggled = toggled;
    mesh_built = true;
    context.draw(mesh);
}

void ControlItemGrid::mouseClickEvent(std::int32_t x,
                                      std::int32_t y,
                                      std::int32_t state,
                                      bool /*still_over_arrow_button*/) {
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
                for (std::int32_t i = 0; i < rows * cols; i++) {
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
                for (std::int32_t i = 0; i < rows * cols; i++) {
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
float ControlItemGrid::getXPos() { return x_pos; }
float ControlItemGrid::getYPos() { return y_pos; }
float ControlItemGrid::getHeight() { return height; }
float ControlItemGrid::getWidth() { return width; }
bool* ControlItemGrid::getSelectedCells() { return &selected_cells[0]; }

void ControlItemGrid::selectCell(std::int32_t row_index,
                                 std::int32_t col_index) {
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
    for (std::int32_t i = 0; i < rows * cols; i++) {
        buttons[i]->setToggled(false);
        buttons[i]->updateButtonState();
        selected_cells[i] = false;
    }
}

void ControlItemGrid::setImageSizeToCell(ImageObject* img, float scale) {
    img->setWidth(cell_width * scale);
    img->setHeight(cell_height * scale);
}

void ControlItemGrid::placeImageToCell(ImageObject* img,
                                       std::int32_t row,
                                       std::int32_t col) {
    img->setXpos(x_pos + (cell_width * col) +
                 ((cell_width - img->getWidth()) / 2.0));
    img->setYpos(y_pos - (cell_height * row) -
                 ((cell_height - img->getHeight()) / 2.0));
    img->setZpos(z_pos + 1);
}

void ControlItemGrid::placeTextToCell(TextObject* text,
                                      std::int32_t row,
                                      std::int32_t col) {
    text->setXpos(x_pos + (cell_width * col) + (cell_width * 0.1));
    text->setYpos(y_pos - (cell_height * row) - (cell_height * 0.85));
    text->setZpos(z_pos + 2);
}

// DUMMY FUNCTIONS
void ControlItemGrid::setOptionText(std::int32_t index) {}
void ControlItemGrid::setOptionText(const std::string& new_text) {}
void ControlItemGrid::updateMouse(std::int32_t x, std::int32_t y) {}
std::string ControlItemGrid::collectData() { return ""; }

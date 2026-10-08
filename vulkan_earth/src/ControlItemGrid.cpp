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
  m_x_pos = new_x_pos;
  m_y_pos = new_y_pos;
  m_z_pos = new_z_pos;
  m_width = new_width;
  m_height = new_height;
  m_rows = new_rows;
  m_cols = new_cols;
  m_cell_width = new_width / (new_cols * 1.0);
  m_cell_height = new_height / (new_rows * 1.0);
  m_active_cell_color[0] = active_cell_color_red;
  m_active_cell_color[1] = active_cell_color_green;
  m_active_cell_color[2] = active_cell_color_blue;
  m_active_cell_color[3] = 1;
  m_visible_lines = new_visible_lines;
  m_multi_selectable = new_multi_selectable;

  m_selected_cells = new bool[new_rows * new_cols];
  m_buttons = new ControlItemButton*[new_rows * new_cols];

  for (std::int32_t i = 0; i < new_rows * new_cols; i++)
    m_selected_cells[i] = false;

  // create buttons and place them in the grid in the order:(0,0), (0,1),
  // (0,2), .....
  std::int32_t button_i = 0;
  for (std::int32_t r = 0; r < new_rows; r++) {
    for (std::int32_t c = 0; c < new_cols; c++) {
      m_buttons[button_i] = new ControlItemButton(nullptr,
          m_x_pos + m_cell_width * c,
          m_y_pos - m_cell_height * r,
          m_z_pos + 0.5,
          0.7,
          0.7,
          0.7,
          m_cell_width,
          m_cell_height,
          "ABC");
      button_i++;
    }
  }
}

ControlItemGrid::~ControlItemGrid() {
  for (std::int32_t i = 0; i < m_rows * m_cols; i++) {
    delete m_buttons[i];
  }
  delete[] m_buttons;
  delete[] m_selected_cells;
}

void ControlItemGrid::draw(render::RenderContext& context) {
  using Vec3 = math::Vec3<float>;
  using Vec4 = math::Vec4<float>;
  std::vector<bool> toggled(m_rows * m_cols);
  for (std::int32_t i = 0; i < m_rows * m_cols; i++) {
    toggled[i] = m_buttons[i]->isToggled();
  }
  if (m_mesh_built && toggled == m_built_toggled) {
    context.draw(m_mesh);
    return;
  }
  m_mesh.clear();

  // Draw main body (sunken bevel: -0.2 top/left, +0.4 bottom/right)
  vulkan_earth::appendFrame(m_mesh,
      m_x_pos,
      m_y_pos,
      m_z_pos + 0.4,
      m_width,
      m_height,
      Vec4(0.75 - 0.2f, 0.75 - 0.2f, 0.75 - 0.2f, 1),
      Vec4(0.45, 0.45, 0.45, 1),
      Vec4(0.75 + 0.4f, 0.75 + 0.4f, 0.75 + 0.4f, 1),
      4);

  // Draw cell lines if they are set to visible
  if (m_visible_lines) {
    const Vec4 black(0, 0, 0, 1);
    for (std::int32_t r = 0; r < m_rows; r++)
      for (std::int32_t c = 0; c < m_cols; c++) {
        // GL_LINE_LOOP: the four edges, closing back to the start.
        const std::array<Vec3, 4> loop = {Vec3(m_x_pos + m_cell_width * c,
                                              m_y_pos - m_cell_height * r,
                                              m_z_pos + 0.5),
            Vec3(m_x_pos + m_cell_width * c,
                m_y_pos - m_cell_height * (r + 1),
                m_z_pos + 0.5),
            Vec3(m_x_pos + m_cell_width * (c + 1),
                m_y_pos - m_cell_height * (r + 1),
                m_z_pos + 0.5),
            Vec3(m_x_pos + m_cell_width * (c + 1),
                m_y_pos - m_cell_height * r,
                m_z_pos + 0.5)};
        for (std::size_t corner = 0; corner < loop.size(); ++corner) {
          m_mesh.addLine(
              loop[corner], loop[(corner + 1) % loop.size()], black);
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
  for (std::int32_t i = 0; i < m_rows * m_cols; i++) {
    if (toggled[i]) {
      m_mesh.addQuad(
          {Vec3(m_buttons[i]->getXPos() + 3,
               m_buttons[i]->getYPos() - 3,
               m_z_pos + 0.6),
              Vec3(m_buttons[i]->getXPos() + 3,
                  m_buttons[i]->getYPos() - m_buttons[i]->getHeight() + 3,
                  m_z_pos + 0.6),
              Vec3(m_buttons[i]->getXPos() + m_buttons[i]->getWidth() - 3,
                  m_buttons[i]->getYPos() - m_buttons[i]->getHeight() + 3,
                  m_z_pos + 0.6),
              Vec3(m_buttons[i]->getXPos() + m_buttons[i]->getWidth() - 3,
                  m_buttons[i]->getYPos() - 3,
                  m_z_pos + 0.6)},
          Vec4(m_active_cell_color[0],
              m_active_cell_color[1],
              m_active_cell_color[2],
              1));
    }
  }
  m_built_toggled = toggled;
  m_mesh_built = true;
  context.draw(m_mesh);
}

void ControlItemGrid::mouseClickEvent(std::int32_t x,
    std::int32_t y,
    std::int32_t state,
    bool /*still_over_arrow_button*/) {
  if (state == 1) {
    // If THE CLICK OCUURED INSIDE OF THE GRID
    if ((m_x_pos <= x && x <= m_x_pos + m_width) &&
        (m_y_pos - m_height <= y && y <= m_y_pos)) {
      playSFX(ITEM_SELECTED);

      // IF THE GRID DOES NOT ALLOW MULTIPLE SELECTION
      if (!m_multi_selectable) {
        // first untoggle all the buttons
        deselectAllCells();
        // find the button that's been clicked on and update the state
        // of the button
        for (std::int32_t i = 0; i < m_rows * m_cols; i++) {
          m_buttons[i]->mouseClickEvent(x, y, state, true);
          if (m_buttons[i]->isToggled()) {
            m_selected_cells[i] = true;
            m_buttons[i]->updateButtonState();
            break;
          }
        }
      }
      // IF THE GRID ALLOWS MULTIPLE SELECTION
      else {
        for (std::int32_t i = 0; i < m_rows * m_cols; i++) {
          if ((x >= (m_buttons[i]->getXPos()) &&
                  x <= ((m_buttons[i]->getXPos()) +
                           (m_buttons[i]->getWidth()))) &&
              (y <= (m_buttons[i]->getYPos()) &&
                  y >= ((m_buttons[i]->getYPos()) -
                           (m_buttons[i]->getHeight())))) {
            m_buttons[i]->mouseClickEvent(x, y, state, true);
            if (m_buttons[i]->isToggled() && !m_selected_cells[i]) {
              m_selected_cells[i] = true;
              m_buttons[i]->updateButtonState();
              break;
            } else if (m_buttons[i]->isToggled() && m_selected_cells[i]) {
              m_buttons[i]->setToggled(false);
              m_selected_cells[i] = false;
              m_buttons[i]->updateButtonState();
              break;
            }
          }
        }
      }
    }
  }
}

// GETTERS & SETTERS
float ControlItemGrid::getXPos() { return m_x_pos; }
float ControlItemGrid::getYPos() { return m_y_pos; }
float ControlItemGrid::getHeight() { return m_height; }
float ControlItemGrid::getWidth() { return m_width; }
bool* ControlItemGrid::getSelectedCells() { return &m_selected_cells[0]; }

void ControlItemGrid::selectCell(
    std::int32_t row_index, std::int32_t col_index) {
  // Check for valid indexing
  if (0 <= row_index && row_index < m_rows && 0 <= col_index &&
      col_index < m_cols) {
    if (!m_multi_selectable) deselectAllCells();
    m_buttons[m_cols * row_index + col_index]->setToggled(true);
    m_buttons[m_cols * row_index + col_index]->updateButtonState();
    m_selected_cells[m_cols * row_index + col_index] = true;
    playSFX(ITEM_SELECTED);
  } else {
    printf(
        "ERROR <ControlItemGrid::selectCell(int, int)>: Wrong "
        "indexing.\n");
  }
}

void ControlItemGrid::deselectAllCells() {
  for (std::int32_t i = 0; i < m_rows * m_cols; i++) {
    m_buttons[i]->setToggled(false);
    m_buttons[i]->updateButtonState();
    m_selected_cells[i] = false;
  }
}

void ControlItemGrid::setImageSizeToCell(ImageObject* img, float scale) {
  img->setWidth(m_cell_width * scale);
  img->setHeight(m_cell_height * scale);
}

void ControlItemGrid::placeImageToCell(
    ImageObject* img, std::int32_t row, std::int32_t col) {
  img->setXpos(m_x_pos + (m_cell_width * col) +
      ((m_cell_width - img->getWidth()) / 2.0));
  img->setYpos(m_y_pos - (m_cell_height * row) -
      ((m_cell_height - img->getHeight()) / 2.0));
  img->setZpos(m_z_pos + 1);
}

void ControlItemGrid::placeTextToCell(
    TextObject* text, std::int32_t row, std::int32_t col) {
  text->setXpos(m_x_pos + (m_cell_width * col) + (m_cell_width * 0.1));
  text->setYpos(m_y_pos - (m_cell_height * row) - (m_cell_height * 0.85));
  text->setZpos(m_z_pos + 2);
}

// DUMMY FUNCTIONS
void ControlItemGrid::setOptionText(std::int32_t index) {}
void ControlItemGrid::setOptionText(const std::string& new_text) {}
void ControlItemGrid::updateMouse(std::int32_t x, std::int32_t y) {}
std::string ControlItemGrid::collectData() { return ""; }

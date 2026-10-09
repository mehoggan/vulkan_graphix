#include "vulkan_earth/Inventory.h"
#include <math.h>
#include <cstdint>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/ControlItemGrid.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/ImageObject.h"
#include "vulkan_earth/Item.h"
#include "vulkan_earth/Player.h"
#include "vulkan_earth/PlayerHuman.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/Weapon.h"

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

Inventory::Inventory() = default;
Inventory::Inventory(float x, float y, std::int32_t w, std::int32_t h) {
  m_x_pos = x - x * 0.01;
  m_y_pos = y - y * 0.01;
  m_width = w + w * 0.01;
  m_height = h + h * 0.01;
  m_inven_grid = new ControlItemGrid(
      -m_width / 4.0,
      m_height / 4.0,
      1,
      m_width * 0.5,
      m_height * 0.4,
      2,
      5,
      0.5,
      0.5,
      1,
      true,
      false);
  for (std::int32_t i = 0; i < player_max_weapons + player_max_items; i++) {
    m_img_inven[i] = nullptr;
    m_remainings[i] = nullptr;
  }
  m_title = new TextObject(
      "Inventory",
      -m_width / 3.3,
      m_height / 2.5,
      1,
      vulkan_earth::FontId::TimesRoman24,
      0,
      0,
      0);
  m_explain = new TextObject(
      "(Press Enter to load/unload a weapon or use an item)",
      -m_width / 3.3,
      m_height / 3.0,
      1,
      vulkan_earth::FontId::TimesRoman24,
      0,
      0,
      0);
  m_descript = new TextObject(
      "",
      -m_width / 3.6,
      -m_height / 3.0,
      1,
      vulkan_earth::FontId::TimesRoman24,
      0,
      0,
      0);
  m_select_cell_row = 0;
  m_select_cell_col = 0;
}
Inventory::~Inventory() {
  delete m_inven_grid;
  for (std::int32_t i = 0; i < player_max_weapons + player_max_items; i++) {
    if (m_img_inven[i]) {
      delete m_img_inven[i];
      delete m_remainings[i];
    }
  }
  delete m_title;
  delete m_explain;
  delete m_descript;
}

void Inventory::setupInventory(Player* player) {
  for (std::int32_t i = 0; i < player_max_weapons; i++) {
    m_weapons[i] = player->getCurrentWeapons()[i];
    m_items[i] = player->getCurrentItems()[i];
  }
  for (std::int32_t i = 0; i < player_max_weapons; i++) {
    // If Player Has A Weapon
    if (m_weapons[i] != nullptr) {
      if (m_img_inven[i]) {
        delete m_img_inven[i];
        delete m_remainings[i];
      }
      m_img_inven[i] = new ImageObject(
          0, 0, 0, 0, 0, 0, 256, 256, m_weapons[i]->getImageFileName());
      m_inven_grid->setImageSizeToCell(m_img_inven[i], 0.8);
      std::string remain = "x " + std::to_string(m_weapons[i]->getRemaining());
      m_remainings[i] = new TextObject(
          remain,
          0,
          0,
          0,
          vulkan_earth::FontId::TimesRoman24,
          0.6f,
          0.3f,
          0.4f);
    } else {
      if (m_img_inven[i]) {
        delete m_img_inven[i];
        delete m_remainings[i];
      }
      m_img_inven[i] = nullptr;
      m_remainings[i] = nullptr;
    }
  }
  for (std::int32_t i = 0; i < player_max_items; i++) {
    // If Player Has An Item
    if (m_items[i] != nullptr) {
      if (m_img_inven[player_max_weapons + i]) {
        delete m_img_inven[player_max_weapons + i];
        delete m_remainings[player_max_weapons + i];
      }
      m_img_inven[player_max_weapons + i] = new ImageObject(
          0, 0, 0, 0, 0, 0, 256, 256, m_items[i]->getImageFileName());
      m_inven_grid->setImageSizeToCell(
          m_img_inven[player_max_weapons + i], 0.8);
      std::string remain = "x " + std::to_string(m_items[i]->getRemaining());
      m_remainings[player_max_weapons + i] = new TextObject(
          remain,
          0,
          0,
          0,
          vulkan_earth::FontId::TimesRoman24,
          0.6f,
          0.3f,
          0.4f);
    } else {
      if (m_img_inven[player_max_weapons + i]) {
        delete m_img_inven[player_max_weapons + i];
        delete m_remainings[player_max_weapons + i];
      }
      m_img_inven[player_max_weapons + i] = nullptr;
      m_remainings[player_max_weapons + i] = nullptr;
    }
  }
  // Place Images and remaining labels to the cells
  std::int32_t index = 0;
  for (std::int32_t r = 0; r < 2; r++) {
    for (std::int32_t c = 0; c < player_max_weapons; c++) {
      if (m_img_inven[index] != nullptr) {
        m_inven_grid->placeImageToCell(m_img_inven[index], r, c);
        m_inven_grid->placeTextToCell(m_remainings[index], r, c);
      }
      index++;
    }
  }
  m_select_cell_row = 0;
  m_select_cell_col = 0;
  m_inven_grid->selectCell(m_select_cell_row, m_select_cell_col);
  if (m_weapons[0] != nullptr) {
    delete m_descript;
    m_descript = new TextObject(
        m_weapons[0]->getDescription(),
        -m_width / 3.6,
        -m_height / 3.0,
        1,
        vulkan_earth::FontId::TimesRoman24,
        0,
        0,
        0);
  } else {
    delete m_descript;
    m_descript = new TextObject(
        "",
        -m_width / 3.6,
        -m_height / 3.0,
        1,
        vulkan_earth::FontId::TimesRoman24,
        0,
        0,
        0);
  }
}

void Inventory::handleInventory(
    Player* current_player, std::int32_t inven_index) {
  // IF PLAYER HAS SHOT WITH A SPECIAL WEAPON
  if (inven_index < player_max_weapons) {
    if (current_player->getLoadedWeapon() != nullptr) {
      if (current_player->getLoadedWeapon()->getRemaining() > 1) {
        current_player->getLoadedWeapon()->setRemaining(
            current_player->getLoadedWeapon()->getRemaining() - 1);
      } else {
        std::int32_t index;
        for (index = 0; index < player_max_weapons; index++) {
          if (current_player->getCurrentWeapons()[index] == nullptr) {
            // do nothing
          } else if (
              current_player->getCurrentWeapons()[index]
                  ->getUNIQUEIDENTIFIER() ==
              current_player->getLoadedWeapon()->getUNIQUEIDENTIFIER()) {
            delete current_player->getLoadedWeapon();
            current_player->getCurrentWeapons()[index] = nullptr;
            current_player->setLoadedWeapon(nullptr);
            break;
          }
        }
      }
    }
  }
  // IF PLAYER HAS USED AN ITEM
  else {
    if (current_player->getCurrentItems()[inven_index - player_max_weapons]
            ->getRemaining() > 1) {
      current_player->getCurrentItems()[inven_index - player_max_weapons]
          ->setRemaining(
              current_player
                  ->getCurrentItems()[inven_index - player_max_weapons]
                  ->getRemaining() -
              1);
    } else {
      delete current_player
          ->getCurrentItems()[inven_index - player_max_weapons];
      current_player->getCurrentItems()[inven_index - player_max_weapons] =
          nullptr;
    }
  }
}

std::int32_t Inventory::getSelectedIndex() {
  return player_max_weapons * m_select_cell_row + m_select_cell_col;
}

void Inventory::keyHandler(std::int32_t key) {
  // LEFT KEY
  if (key == 1 && m_select_cell_col > 0) {
    m_select_cell_col--;
    m_inven_grid->selectCell(m_select_cell_row, m_select_cell_col);
  }
  // UP KEY
  else if (key == 2 && m_select_cell_row > 0) {
    m_select_cell_row--;
    m_inven_grid->selectCell(m_select_cell_row, m_select_cell_col);
  }
  // RIGHT KEY
  else if (key == 3 && m_select_cell_col < player_max_weapons - 1) {
    m_select_cell_col++;
    m_inven_grid->selectCell(m_select_cell_row, m_select_cell_col);
  }
  // DOWN KEY
  else if (key == 4 && m_select_cell_row < 1) {
    m_select_cell_row++;
    m_inven_grid->selectCell(m_select_cell_row, m_select_cell_col);
  }

  // IF ONE OF WEAPONS IS SELECTED
  if (m_select_cell_row == 0) {
    if (m_weapons[m_select_cell_col] != nullptr) {
      delete m_descript;
      m_descript = new TextObject(
          m_weapons[m_select_cell_col]->getDescription(),
          -m_width / 3.6,
          -m_height / 3.0,
          1,
          vulkan_earth::FontId::TimesRoman24,
          0,
          0,
          0);
    } else {
      delete m_descript;
      m_descript = new TextObject(
          "",
          -m_width / 3.6,
          -m_height / 3.0,
          1,
          vulkan_earth::FontId::TimesRoman24,
          0,
          0,
          0);
    }
  }
  // IF ONE OF ITEMS IS SELECTED
  else {
    if (m_items[m_select_cell_col] != nullptr) {
      delete m_descript;
      m_descript = new TextObject(
          m_items[m_select_cell_col]->getDescription(),
          -m_width / 3.6,
          -m_height / 3.0,
          1,
          vulkan_earth::FontId::TimesRoman24,
          0,
          0,
          0);
    } else {
      delete m_descript;
      m_descript = new TextObject(
          "",
          -m_width / 3.6,
          -m_height / 3.0,
          1,
          vulkan_earth::FontId::TimesRoman24,
          0,
          0,
          0);
    }
  }
}

void Inventory::draw(render::RenderContext& context) {
  // glViewport()'s float -> int truncation kept.
  vulkan_earth::beginOverlayPanel(
      context,
      vulkan_earth::glRect(
          static_cast<std::int32_t>(m_x_pos),
          static_cast<std::int32_t>(m_y_pos),
          m_width,
          m_height),
      m_width,
      m_height);

  m_inven_grid->draw(context);
  m_title->draw(context);
  m_explain->draw(context);
  m_descript->draw(context);
  for (std::int32_t i = 0; i < player_max_weapons + player_max_items; i++) {
    if (m_img_inven[i] != nullptr) {
      m_img_inven[i]->draw(context);
      m_remainings[i]->draw(context);
    }
  }

  vulkan_earth::resetToFullWindow(context);
}

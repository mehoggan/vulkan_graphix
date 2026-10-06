#include "vulkan_earth/ReadyMenu.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/ControlItemSelectionBox.h"
#include "vulkan_earth/ControlItemSliderbar.h"
#include "vulkan_earth/ControlItemTextField.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/GlobalSettings.h"
#include "vulkan_earth/ImageObject.h"
#include "vulkan_earth/MainMenu.h"
#include "vulkan_earth/MainMenuButton.h"
#include "vulkan_earth/Player.h"
#include "vulkan_earth/PlayerCPU.h"
#include "vulkan_earth/PlayerFactory.h"
#include "vulkan_earth/PlayerHuman.h"
#include "vulkan_earth/PossibleGameStates.h"
#include "vulkan_earth/ShopMenu.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/Tank.h"
#include "vulkan_earth/TankA.h"
#include "vulkan_earth/TankB.h"
#include "vulkan_earth/TankC.h"
#include "vulkan_earth/TankD.h"
#include "vulkan_earth/TankE.h"
#include "vulkan_earth/TankF.h"
#include "vulkan_earth/TankG.h"
#include "vulkan_earth/TankH.h"
#include "vulkan_earth/TerrainMaker.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Tools.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playMusic(std::int32_t music);

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

ReadyMenu::ReadyMenu() = default;
ReadyMenu::ReadyMenu(float new_width,
    float new_height,
    float new_percent_border,
    GlobalSettings* new_global_settings,
    PlayerFactory* new_player_factory,
    std::int32_t* game_state) {
  m_start_music_played = false;
  m_global_settings = new_global_settings;
  m_player_factory = new_player_factory;
  m_current_game_state = game_state;

  m_num_players = max_num_players;
  for (std::int32_t i = 0; i < num_buttons; i++) {
    m_buttons[i] = nullptr;
  }
  for (std::int32_t i = 0; i < num_stat_images; i++) {
    m_stat_images[i] = nullptr;
  }
  for (std::int32_t i = 0; i < num_control_items; i++) {
    m_control_items[i] = nullptr;
  }

  m_button_pressed = nullptr;
  m_width = new_width;
  m_height = new_height;
  m_percent_border = new_percent_border;
  m_pos[0] = m_pos[1] = m_pos[2] = 0;
  m_color[0] = m_color[1] = m_color[2] = 1;
  m_color[3] = 1;
  m_tank_prv_scr_pos[0] = m_width * 0.07;
  m_tank_prv_scr_pos[1] = m_height * 0.40;
  m_tank_prv_scr_pos[2] = 0.1;
  m_tank_prv_scr_width = m_width * 0.35;
  m_tank_prv_scr_height = m_height * 0.45;
  m_tank_prv_scr_color[0] = m_tank_prv_scr_color[1] = m_tank_prv_scr_color[2] =
      0;
  m_prv_scr_color_control = 1;

  m_caption = "Player 1";
  m_current_player_index = 0;

  /*BUTTONS AND CONTROL ITEMS*/
  m_buttons[0] = new MainMenuButton(0,
      m_pos[0] - m_width * 0.35,
      m_pos[1] + m_height * 0.35,
      1,
      m_player_factory->collectPlayerColor(m_current_player_index)[0],
      m_player_factory->collectPlayerColor(m_current_player_index)[1],
      m_player_factory->collectPlayerColor(m_current_player_index)[2],
      0.1 * (m_width),
      0.04 * (m_height),
      "CPU",
      nullptr);

  m_buttons[0]->pressButton();
  Mix_HaltChannel(0);
  m_buttons[0]->activateSubMenu();
  m_buttons[1] = new MainMenuButton(1,
      m_pos[0] - m_width * 0.2,
      m_pos[1] + m_height * 0.35,
      1,
      m_player_factory->collectPlayerColor(m_current_player_index)[0],
      m_player_factory->collectPlayerColor(m_current_player_index)[1],
      m_player_factory->collectPlayerColor(m_current_player_index)[2],
      0.1 * (m_width),
      0.04 * (m_height),
      "HUMAN",
      nullptr);
  m_buttons[2] = new MainMenuButton(2,
      m_pos[0] + m_width * 0.125,
      m_pos[1] - m_height * 0.35,
      1,
      0.65,
      0.15,
      0.15,
      0.1 * (m_width),
      0.04 * (m_height),
      "Back",
      nullptr);
  m_buttons[3] = new MainMenuButton(3,
      m_pos[0] + m_width * 0.275,
      m_pos[1] - m_height * 0.35,
      1,
      0.75,
      0.75,
      0.75,
      0.1 * (m_width),
      0.04 * (m_height),
      "Next",
      nullptr);
  m_control_items[0] = new ControlItemSelectionBox(m_pos[0] - m_width * 0.375,
      m_pos[1] + m_height * 0.275,
      1,
      0.55,
      0.55,
      0.55,
      0.175 * (m_width),
      0.04 * (m_height),
      "AI",
      "Moron/Tosser/Cyborg/Shooter/Chooser/Poolshark/Spoiler/Unknown/");
  m_control_items[1] = new ControlItemSliderbar(m_pos[0] - m_width * 0.35,
      m_pos[1] - m_height * 0.24,
      1,
      0.55,
      0.55,
      0.55,
      0.7 * (m_width),
      0.06 * (m_height),
      "Tank Type",
      "Rhinoxx/Hellfire/HeavyD/Panzer/Eggroid/Behemoth/Cubix/Predator/",
      0);
  m_control_items[2] = new ControlItemSelectionBox(m_pos[0] - m_width * 0.175,
      m_pos[1] + m_height * 0.275,
      1,
      0.55,
      0.55,
      0.55,
      0.1 * (m_width),
      0.04 * (m_height),
      "Team",
      "-/1/2/3/4/5/");
  m_text_field = new ControlItemTextField(m_pos[0] - m_width * 0.375,
      m_pos[1] + m_height * 0.275,
      1,
      1.0,
      1.0,
      1.0,
      0.175 * (m_width),
      0.04 * (m_height));

  /*LABEL PLACEMENT*/
  m_player_page_num = new TextObject(m_caption,
      m_pos[0] - m_width * 0.25,
      m_pos[1] + m_height * 0.4,
      (m_pos[2] + 1),
      vulkan_earth::FontId::TimesRoman24,
      0.0f,
      0.0f,
      0.0f);

  for (std::int32_t i = 0; i < num_tank_stats; i++) {
    std::string stat;
    if (i == 0)
      stat = "Power:";
    else if (i == 1)
      stat = "Armor:";
    else if (i == 2)
      stat = "Speed:";
    else
      stat = "Meh...:";
    float stat_label_x_pos = m_pos[0] - m_width * 0.385;
    float stat_label_y_pos =
        m_pos[1] + m_height * 0.12 - m_height * (i * 0.07);
    m_tank_stat_labels[i] = new TextObject(stat,
        stat_label_x_pos,
        stat_label_y_pos,
        (m_pos[2] + 1),
        vulkan_earth::FontId::TimesRoman24,
        0.0f,
        0.0f,
        0.0f);
  }

  // TANKS
  m_tanks[0] = new TankA(0, 0, 0);
  m_tanks[1] = new TankB(0, 0, 0);
  m_tanks[2] = new TankC(0, 0, 0);
  m_tanks[3] = new TankD(0, 0, 0);
  m_tanks[4] = new TankE(0, 0, 0);
  m_tanks[5] = new TankF(0, 0, 0);
  m_tanks[6] = new TankG(0, 0, 0);
  m_tanks[7] = new TankH(0, 0, 0);
  m_tank_angle = 0;

  for (std::int32_t i = 0; i < num_tank_types; i++) {
    m_tanks[i]->setTankPos(0, 0, 0);
  }

  // STAT IMAGES
  float img_start_pos_x = m_pos[0] - m_width * 0.325;
  for (std::int32_t i = 0; i < num_stat_images; i++) {
    // For Off Lights
    if (i < 30) {
      // For Power Lights
      if (i < 10) {
        m_stat_images[i] =
            new ImageObject(img_start_pos_x + m_width * (i % 10) * 0.03,
                m_pos[1] + new_height * 0.14 - m_height * ((i / 10) * 0.07),
                m_pos[2] + 0.5,
                m_width * 0.03,
                m_height * 0.032,
                0.0006 * (m_width),
                64,
                64,
                "lightOff.raw");
      }
      // For Armor Lights
      else if (i < 20) {
        m_stat_images[i] =
            new ImageObject(img_start_pos_x + m_width * (i % 10) * 0.03,
                m_pos[1] + new_height * 0.14 - m_height * ((i / 10) * 0.07),
                m_pos[2] + 0.5,
                m_width * 0.03,
                m_height * 0.032,
                0.0006 * (m_width),
                64,
                64,
                "lightOff.raw");
      }
      // For Speed Lights
      else {
        m_stat_images[i] =
            new ImageObject(img_start_pos_x + m_width * (i % 10) * 0.03,
                m_pos[1] + new_height * 0.14 - m_height * ((i / 10) * 0.07),
                m_pos[2] + 0.5,
                m_width * 0.03,
                m_height * 0.032,
                0.0006 * (m_width),
                64,
                64,
                "lightOff.raw");
      }
    }
    // For On Lights
    else {
      // For Power Lights
      if (i < 40) {
        m_stat_images[i] = new ImageObject(
            img_start_pos_x + m_width * (i % 10) * 0.03,
            m_pos[1] + new_height * 0.14 - m_height * (((i - 30) / 10) * 0.07),
            m_pos[2] + 1,
            m_width * 0.03,
            m_height * 0.032,
            0.0006 * (m_width),
            64,
            64,
            "lightRed.raw");
      }
      // For Armor Lights
      else if (i < 50) {
        m_stat_images[i] = new ImageObject(
            img_start_pos_x + m_width * (i % 10) * 0.03,
            m_pos[1] + new_height * 0.14 - m_height * (((i - 30) / 10) * 0.07),
            m_pos[2] + 1,
            m_width * 0.03,
            m_height * 0.032,
            0.0006 * (m_width),
            64,
            64,
            "lightBlue.raw");
      }
      // For Speed Lights
      else {
        m_stat_images[i] = new ImageObject(
            img_start_pos_x + m_width * (i % 10) * 0.03,
            m_pos[1] + new_height * 0.14 - m_height * (((i - 30) / 10) * 0.07),
            m_pos[2] + 1,
            m_width * 0.03,
            m_height * 0.032,
            0.0006 * (m_width),
            64,
            64,
            "lightGreen.raw");
      }
    }
  }

  updatePageInfo();
}

ReadyMenu::~ReadyMenu() {
  for (std::int32_t i = 0; i < num_buttons; i++) delete m_buttons[i];
  for (std::int32_t i = 0; i < num_stat_images; i++) delete m_stat_images[i];
  for (std::int32_t i = 0; i < num_control_items; i++)
    delete m_control_items[i];
  for (std::int32_t i = 0; i < num_tank_types; i++) delete m_tanks[i];
  for (std::int32_t i = 0; i < num_tank_stats; i++)
    delete m_tank_stat_labels[i];
  delete m_text_field;
  delete m_player_page_num;
}

// GETTERS & SETTERS //
float* ReadyMenu::getPos() { return &(m_pos[0]); }
float ReadyMenu::getWidth() { return m_width; }
float ReadyMenu::getHeight() { return m_height; }
float* ReadyMenu::getColor() { return &(m_color[0]); }
void ReadyMenu::setWidth(float new_width) { m_width = new_width; }
void ReadyMenu::setHeight(float new_height) { m_height = new_height; }
void ReadyMenu::updateNumPlayers(std::int32_t n) { m_num_players = n; }
void ReadyMenu::setColor(float r, float g, float b, float a) {
  m_color[0] = r;
  m_color[1] = g;
  m_color[2] = b;
  m_color[3] = a;
}

void ReadyMenu::saveCurrentPlayerData() {
  std::string aitype = m_control_items[0]->collectData();
  std::string name = m_text_field->collectData();
  std::string tank = m_control_items[1]->collectData();
  char team_label = (m_control_items[2]->collectData()[0]);

  if (m_buttons[0]->isActive()) {
    m_player_factory->updatePlayerBasicStrings(
        "CPU", aitype, "", team_label, tank, m_current_player_index);
  } else {
    m_player_factory->updatePlayerBasicStrings(
        "HUMAN", "", name, team_label, tank, m_current_player_index);
    m_text_field->clearTextBuffer();
    m_text_field->deactivate();
  }
}

void ReadyMenu::showPreviousPlayerPage() {
  if (m_current_player_index == 0) {
    Mix_FadeOutMusic(300);
    Mix_HaltMusic();
    *m_current_game_state = MAIN_MENU;
    m_start_music_played = false;

  } else {
    if (m_current_player_index == 1) {
      m_buttons[2]->setColor(0.65, 0.15, 0.15);
      m_text_field->setTextBuffer("");
    }
    m_buttons[3]->setLabel("Next");
    m_buttons[3]->setColor(0.75, 0.75, 0.75);
    m_current_player_index--;
    setPlayerPageNum(m_current_player_index);
    m_text_field->setTextBuffer("");
    updatePageInfo();
  }
}

void ReadyMenu::showNextPlayerPage() {
  if (m_current_player_index + 1 == m_num_players) {
    Mix_FadeOutMusic(300);
    Mix_HaltMusic();
    *m_current_game_state = SHOP_MENU;
    m_text_field->setTextBuffer("");
  } else {
    if (m_current_player_index + 1 == m_num_players - 1) {
      m_buttons[3]->setLabel("Done");
      m_buttons[3]->setColor(0.65, 0.15, 0.15);
    }
    m_buttons[2]->setColor(0.75, 0.75, 0.75);
    m_current_player_index++;
    setPlayerPageNum(m_current_player_index);
    m_text_field->setTextBuffer("");
    updatePageInfo();
  }
}

void ReadyMenu::updatePageInfo() {
  for (std::int32_t i = 0; i < num_tank_types; i++) {
    m_tanks[i]->changeHeadTexture(m_current_player_index);
  }
  std::string player_type =
      m_player_factory->getPlayer(m_current_player_index)->getPlayerType();

  // test to see if it's CPU
  if (player_type == "CPU") {
    m_buttons[0]->pressButton();
    Mix_HaltChannel(0);
    m_buttons[0]->activateSubMenu();
    m_buttons[1]->depressButton();
    m_buttons[1]->deactivateSubMenu();

    std::string ai_type =
        m_player_factory->getPlayer(m_current_player_index)->getAiType();
    std::int32_t i = 0;
    while (m_control_items[0]->collectData() != ai_type) {
      m_control_items[0]->setOptionText(i);
      i++;
    }
  } else {
    std::string name =
        m_player_factory->getPlayer(m_current_player_index)->getPlayerName();
    m_buttons[1]->pressButton();
    Mix_HaltChannel(0);
    m_buttons[1]->activateSubMenu();
    m_buttons[0]->depressButton();
    m_buttons[0]->deactivateSubMenu();

    m_text_field->setTextBuffer(name);
  }

  if (m_player_factory->getPlayer(m_current_player_index)->getTeamLabel() ==
      '-') {
    m_control_items[2]->setOptionText(0);
  } else {
    m_control_items[2]->setOptionText(
        m_player_factory->getPlayer(m_current_player_index)->getTeamLabel() -
        48);
  }

  std::string tank_type =
      m_player_factory->getPlayer(m_current_player_index)->getTankType();
  std::int32_t i = 0;
  while (m_control_items[1]->collectData() != tank_type) {
    m_control_items[1]->setOptionText(i);
    i++;
  }

  m_buttons[0]->setColor(
      m_player_factory->collectPlayerColor(m_current_player_index)[0],
      m_player_factory->collectPlayerColor(m_current_player_index)[1],
      m_player_factory->collectPlayerColor(m_current_player_index)[2]);
  m_buttons[1]->setColor(
      m_player_factory->collectPlayerColor(m_current_player_index)[0],
      m_player_factory->collectPlayerColor(m_current_player_index)[1],
      m_player_factory->collectPlayerColor(m_current_player_index)[2]);

  m_tank_prv_scr_color[0] =
      m_player_factory->collectPlayerColor(m_current_player_index)[0];
  m_tank_prv_scr_color[1] =
      m_player_factory->collectPlayerColor(m_current_player_index)[1];
  m_tank_prv_scr_color[2] =
      m_player_factory->collectPlayerColor(m_current_player_index)[2];
}

void ReadyMenu::setPlayerPageNum(std::int32_t i) {
  delete m_player_page_num;
  m_caption = "Player " + std::to_string(i + 1);
  float label_x_pos = m_pos[0] - m_width * 0.25;
  float label_y_pos = m_pos[1] + m_height * 0.4;
  m_player_page_num = new TextObject(m_caption,
      label_x_pos,
      label_y_pos,
      (m_pos[2] + 1),
      vulkan_earth::FontId::TimesRoman24,
      0.0f,
      0.0f,
      0.0f);
}

void ReadyMenu::buttonTest(
    std::int32_t x, std::int32_t y, std::int32_t button_down) {
  if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
    for (std::int32_t button_i = 0; button_i < num_buttons;
        button_i++) {             // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                                  // IF CLICK LANDS IN BUTTON I
      if (m_buttons[button_i]) {  // JUST TO MAKE SURE
        if ((x >= m_buttons[button_i]->getXPos()) &&
            (x <= (m_buttons[button_i]->getXPos() +
                      m_buttons[button_i]->getWidth())) &&
            (y <= m_buttons[button_i]->getYPos()) &&
            (y >= (m_buttons[button_i]->getYPos() -
                      m_buttons[button_i]->getHeight()))) {
          m_buttons[button_i]->pressButton();      // PRESS BUTTON
          m_button_pressed = m_buttons[button_i];  // KEEP TRACK OF WHICH
                                                   // BUTTON WAS PRESSED
        }
      }
    }
    for (std::int32_t control_i = 0; control_i < num_control_items;
        control_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                        // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                        // CLICKED A ARROW BUTTON???
      if ((x >= m_control_items[control_i]->getXPos()) &&
          (x <= (m_control_items[control_i]->getXPos() +
                    m_control_items[control_i]->getWidth())) &&
          (y <= m_control_items[control_i]->getYPos()) &&
          (y >= (m_control_items[control_i]->getYPos() -
                    m_control_items[control_i]->getHeight()))) {
        m_control_items[control_i]->mouseClickEvent(x,
            y,
            button_down,
            true);  // YOU PRESSED OVER A ARROWBUTTON
      }
    }
    if ((x >= m_text_field->getXPos()) &&
        (x <= (m_text_field->getXPos() + m_text_field->getWidth())) &&
        (y <= m_text_field->getYPos()) &&
        (y >= (m_text_field->getYPos() - m_text_field->getHeight()))) {
      m_text_field->mouseClickEvent(x, y, button_down, true);
    } else {
      m_text_field->mouseClickEvent(x, y, button_down, false);
    }
  }

  else if (!button_down) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                            // GOES UP
    if (m_text_field->isTextFieldActive()) m_text_field->deactivate();
    if (m_button_pressed !=
        nullptr) {  // IF THE LEFT CLICK WAS VALID AND INSIDE A BUTTON
                    // CHECK TO SEE IF YOU ARE STILL OVER SAME BUTTON
      if ((x >= m_button_pressed->getXPos()) &&
          (x <=
              (m_button_pressed->getXPos() + m_button_pressed->getWidth())) &&
          (y <= m_button_pressed->getYPos()) &&
          (y >=
              (m_button_pressed->getYPos() - m_button_pressed->getHeight()))) {
        if (m_button_pressed->getUNIQUEIDENTIFIER() ==
            0) {  // YOU CLICKED CPU TOGGLE BUTTON
          m_buttons[1]->depressButton();
          m_buttons[1]->deactivateSubMenu();
          m_buttons[0]->activateSubMenu();
          m_tanks[5]->getBaseHP();
          m_button_pressed = nullptr;
        } else if (m_button_pressed->getUNIQUEIDENTIFIER() ==
            1) {  // YOU CLICKED HUMAN TOGGLE BUTTON
          m_buttons[0]->deactivateSubMenu();
          m_buttons[0]->depressButton();
          m_buttons[1]->activateSubMenu();
          m_button_pressed = nullptr;
        } else if (m_button_pressed->getUNIQUEIDENTIFIER() ==
            2) {  // YOU CLICKED BACK BUTTON
          saveCurrentPlayerData();
          showPreviousPlayerPage();
          m_button_pressed->depressButton();
          m_button_pressed = nullptr;
        } else if (m_button_pressed->getUNIQUEIDENTIFIER() ==
            3) {  // YOU CLICKED NEXT BUTTON
          saveCurrentPlayerData();
          showNextPlayerPage();
          m_button_pressed->depressButton();
          m_button_pressed = nullptr;
        }
      } else {  // IF YOU RELEASE OUTSIDE ALL BUTTONS
        if (!m_button_pressed->isActive())  // IF THE BUTTON PRESSED IS NOT THE
                                            // BUTTON THAT'S TOGGLED
        {
          m_button_pressed->depressButton();
          m_button_pressed = nullptr;
        }
      }
    }
    for (std::int32_t control_i = 0; control_i < num_control_items;
        control_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                        // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                        // CLICKED A ARROW BUTTON???
      if ((x >= m_control_items[control_i]->getXPos()) &&
          (x <= (m_control_items[control_i]->getXPos() +
                    m_control_items[control_i]->getWidth())) &&
          (y <= m_control_items[control_i]->getYPos()) &&
          (y >= (m_control_items[control_i]->getYPos() -
                    m_control_items[control_i]->getHeight()))) {
        if (control_i == 0 && m_buttons[control_i + 1]->isActive())
          ;  // IF HUMAN BUTTON IS TOGGLED, SELECTION BUTTON SHOULD
             // DO NOTHING
        else {
          m_control_items[control_i]->mouseClickEvent(x, y, button_down, true);
        }
      } else {
        if (control_i == 0 && m_buttons[control_i + 1]->isActive())
          ;  // IF HUMAN BUTTON IS TOGGLED, SELECTION BUTTON SHOULD
             // DO NOTHING
        else {
          m_control_items[control_i]->mouseClickEvent(
              x, y, button_down, false);
        }
      }
    }
    if ((x >= m_text_field->getXPos()) &&
        (x <= (m_text_field->getXPos() + m_text_field->getWidth())) &&
        (y <= m_text_field->getYPos()) &&
        (y >= (m_text_field->getYPos() - m_text_field->getHeight()))) {
      m_text_field->mouseClickEvent(x,
          y,
          button_down,
          true);  // YOU PRESSED OVER A ARROWBUTTON
    }
  }
}

void ReadyMenu::updateMouse(std::int32_t x, std::int32_t y) {
  m_control_items[1]->updateMouse(x, y);
}

void ReadyMenu::draw(render::RenderContext& context) {
  if (!m_start_music_played) {
    playMusic(readymenu_start);
    m_start_music_played = true;
  }
  playMusic(readymenu_loop);
  using Vec3 = math::Vec3<float>;
  using Vec4 = math::Vec4<float>;
  // The whole-window background panel (see appendMenuPanel()), then the
  // tank preview screen's 6-pixel frame: top 0.45, left 0.4, bottom 0.8,
  // right 0.85 (its middle pane is the preview viewport drawn below).
  if (m_panel_mesh.triangles().empty() || m_built_width != m_width ||
      m_built_height != m_height) {
    m_panel_mesh.clear();
    vulkan_earth::appendMenuPanel(
        m_panel_mesh, m_width, m_height, m_percent_border);
    const float x = m_tank_prv_scr_pos[0];
    const float y = m_tank_prv_scr_pos[1];
    const float z = m_tank_prv_scr_pos[2];
    m_panel_mesh.addQuad({Vec3(x, y, z),
                             Vec3(x - 6, y + 6, z),
                             Vec3(x + m_tank_prv_scr_width + 6, y + 6, z),
                             Vec3(x + m_tank_prv_scr_width, y, z)},
        Vec4(0.45, 0.45, 0.45, 1));
    m_panel_mesh.addQuad({Vec3(x - 6, y + 6, z),
                             Vec3(x - 6, y - m_tank_prv_scr_height - 6, z),
                             Vec3(x, y - m_tank_prv_scr_height, z),
                             Vec3(x, y, z)},
        Vec4(0.4, 0.4, 0.4, 1));
    m_panel_mesh.addQuad(
        {Vec3(x - 6, y - m_tank_prv_scr_height - 6, z),
            Vec3(x + m_tank_prv_scr_width + 6,
                y - m_tank_prv_scr_height - 6,
                z),
            Vec3(x + m_tank_prv_scr_width, y - m_tank_prv_scr_height, z),
            Vec3(x, y - m_tank_prv_scr_height, z)},
        Vec4(0.8, 0.8, 0.8, 1));
    m_panel_mesh.addQuad(
        {Vec3(x + m_tank_prv_scr_width, y, z),
            Vec3(x + m_tank_prv_scr_width + 6, y + 6, z),
            Vec3(x + m_tank_prv_scr_width + 6,
                y - m_tank_prv_scr_height - 6,
                z),
            Vec3(x + m_tank_prv_scr_width, y + -m_tank_prv_scr_height, z)},
        Vec4(0.85, 0.85, 0.85, 1));
    m_built_width = m_width;
    m_built_height = m_height;
  }
  context.draw(m_panel_mesh);

  for (std::int32_t i = 0; i < num_tank_stats; i++) {
    m_tank_stat_labels[i]->draw(context);
  }
  for (std::int32_t i = 0; i < num_buttons; i++) {
    m_buttons[i]->draw(context);
  }
  if (m_buttons[0]->isActive()) {
    m_control_items[0]->draw(context);
  } else {
    m_text_field->draw(context);
  }
  m_control_items[1]->draw(context);
  m_control_items[2]->draw(context);
  m_player_page_num->draw(context);

  // Flashing color effect in the tank preview screen
  if (0 <= m_tank_prv_scr_color[0] &&
      m_tank_prv_scr_color[0] <=
          m_player_factory->collectPlayerColor(m_current_player_index)[0] *
              1.12)
    m_tank_prv_scr_color[0] +=
        (m_tank_prv_scr_color[0] + 0.1) / 100 * m_prv_scr_color_control;
  if (0 <= m_tank_prv_scr_color[1] &&
      m_tank_prv_scr_color[1] <=
          m_player_factory->collectPlayerColor(m_current_player_index)[1] *
              1.12)
    m_tank_prv_scr_color[1] +=
        (m_tank_prv_scr_color[1] + 0.1) / 100 * m_prv_scr_color_control;
  if (0 <= m_tank_prv_scr_color[2] &&
      m_tank_prv_scr_color[2] <=
          m_player_factory->collectPlayerColor(m_current_player_index)[2] *
              1.12)
    m_tank_prv_scr_color[2] +=
        (m_tank_prv_scr_color[2] + 0.1) / 100 * m_prv_scr_color_control;
  if (m_tank_prv_scr_color[0] + m_tank_prv_scr_color[1] +
          m_tank_prv_scr_color[2] <
      0) {
    m_tank_prv_scr_color[0] = m_tank_prv_scr_color[1] =
        m_tank_prv_scr_color[2] = 0;
    m_prv_scr_color_control = 1;
  }

  if (m_tank_prv_scr_color[0] + m_tank_prv_scr_color[1] +
          m_tank_prv_scr_color[2] >
      (m_player_factory->collectPlayerColor(m_current_player_index)[0] +
          m_player_factory->collectPlayerColor(m_current_player_index)[1] +
          m_player_factory->collectPlayerColor(m_current_player_index)[2]) *
          1.12) {
    m_tank_prv_scr_color[0] =
        m_player_factory->collectPlayerColor(m_current_player_index)[0] * 1.11;
    m_tank_prv_scr_color[1] =
        m_player_factory->collectPlayerColor(m_current_player_index)[1] * 1.11;
    m_tank_prv_scr_color[2] =
        m_player_factory->collectPlayerColor(m_current_player_index)[2] * 1.11;
    m_prv_scr_color_control = -1;
  }
  // Draw Stat Images
  for (std::int32_t i = 0; i < num_stat_images; i++) {
    if (i < 30) {
      m_stat_images[i]->draw(context);
    } else {
      if (i < 40) {
        if (m_control_items[1]->collectData() == "Rhinoxx" &&
            i - 30 < m_tanks[0]->getBasePower()) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Hellfire" &&
            i - 30 < m_tanks[1]->getBasePower()) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "HeavyD" &&
            i - 30 < m_tanks[2]->getBasePower()) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Panzer" &&
            i - 30 < m_tanks[3]->getBasePower()) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Eggroid" &&
            i - 30 < m_tanks[4]->getBasePower()) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Behemoth" &&
            i - 30 < m_tanks[5]->getBasePower()) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Cubix" &&
            i - 30 < m_tanks[6]->getBasePower()) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Predator" &&
            i - 30 < m_tanks[7]->getBasePower()) {
          m_stat_images[i]->draw(context);
        }
      } else if (i < 50) {
        if (m_control_items[1]->collectData() == "Rhinoxx" &&
            i - 40 < m_tanks[0]->getBaseArmor()) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Hellfire" &&
            i - 40 < m_tanks[1]->getBaseArmor()) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "HeavyD" &&
            i - 40 < m_tanks[2]->getBaseArmor()) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Panzer" &&
            i - 40 < m_tanks[3]->getBaseArmor()) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Eggroid" &&
            i - 40 < m_tanks[4]->getBaseArmor()) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Behemoth" &&
            i - 40 < m_tanks[5]->getBaseArmor()) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Cubix" &&
            i - 40 < m_tanks[6]->getBaseArmor()) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Predator" &&
            i - 40 < m_tanks[7]->getBaseArmor()) {
          m_stat_images[i]->draw(context);
        }
      } else {
        if (m_control_items[1]->collectData() == "Rhinoxx" &&
            i - 50 < m_tanks[0]->getBaseSpeed() / 10) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Hellfire" &&
            i - 50 < m_tanks[1]->getBaseSpeed() / 10) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "HeavyD" &&
            i - 50 < m_tanks[2]->getBaseSpeed() / 10) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Panzer" &&
            i - 50 < m_tanks[3]->getBaseSpeed() / 10) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Eggroid" &&
            i - 50 < m_tanks[4]->getBaseSpeed() / 10) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Behemoth" &&
            i - 50 < m_tanks[5]->getBaseSpeed() / 10) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Cubix" &&
            i - 50 < m_tanks[6]->getBaseSpeed() / 10) {
          m_stat_images[i]->draw(context);
        } else if (m_control_items[1]->collectData() == "Predator" &&
            i - 50 < m_tanks[7]->getBaseSpeed() / 10) {
          m_stat_images[i]->draw(context);
        }
      }
    }
  }

  // The live tank preview, in its own viewport (glViewport()'s float ->
  // int truncation kept), cleared to the flashing preview color.
  const render::Rect preview = vulkan_earth::glRect(
      static_cast<std::int32_t>(m_tank_prv_scr_pos[0] + getWidth() / 2),
      static_cast<std::int32_t>(m_tank_prv_scr_height + 1),
      static_cast<std::int32_t>(m_tank_prv_scr_width),
      static_cast<std::int32_t>(m_tank_prv_scr_height));
  context.setViewport(preview);
  context.clearColorAndDepth(Vec4(m_tank_prv_scr_color[0],
      m_tank_prv_scr_color[1],
      m_tank_prv_scr_color[2],
      1));

  // gluLookAt(	0,0,400,	0, 0, 0,		0.0f,1.0f,0.0f);
  math::Mat4<float> view =
      glm::lookAt(Vec3(0, 200, 500), Vec3(0, 0, 0), Vec3(0.0f, 1.0f, 0.0f));

  // Draw Tanks
  view = glm::translate(view,
      math::Vec3<float>(
          m_pos[0], m_pos[1] - m_tank_prv_scr_height * 0.2, m_pos[2]));
  view = glm::rotate(view,
      glm::radians(static_cast<float>(m_tank_angle)),
      math::Vec3<float>(0, 1, 0));
  context.setCamera(
      vulkan_graphix::Tools::getPerspectiveProjectionMatrix(
          (static_cast<float>(m_width) / (1.5 * static_cast<float>(m_height))),
          60.0,
          1,
          2.0e8f),
      view);
  if (m_control_items[1]->collectData() == "Rhinoxx")
    m_tanks[0]->draw(context);
  else if (m_control_items[1]->collectData() == "Hellfire")
    m_tanks[1]->draw(context);
  else if (m_control_items[1]->collectData() == "HeavyD")
    m_tanks[2]->draw(context);
  else if (m_control_items[1]->collectData() == "Panzer")
    m_tanks[3]->draw(context);
  else if (m_control_items[1]->collectData() == "Eggroid")
    m_tanks[4]->draw(context);
  else if (m_control_items[1]->collectData() == "Behemoth")
    m_tanks[5]->draw(context);
  else if (m_control_items[1]->collectData() == "Cubix")
    m_tanks[6]->draw(context);
  else if (m_control_items[1]->collectData() == "Predator")
    m_tanks[7]->draw(context);
  else {
    printf("ERROR: Unkown tank type\n");
    // glutSolidSphere(100, 30, 30)
    context.drawMesh(render::Renderer::instance().sphere(30, 30),
        vulkan_earth::pipelines().m_flat_color,
        nullptr,
        glm::scale(math::Mat4<float>(1.0f), math::Vec3<float>(100, 100, 100)),
        Vec4(1, 1, 1, 1));
  }

  m_tank_angle += 0.25f;

  vulkan_earth::resetToFullWindow(context);
}

void ReadyMenu::keyTest(std::uint8_t key) {
  if (m_text_field->isTextFieldActive()) {
    m_text_field->keyHandler(key);
  }
}
#include "vulkan_earth/MainMenu.h"
#include <math.h>
#include <stdlib.h>
#include <cstdint>
#include <string>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/ControlItemSelectionBox.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/GlobalSettings.h"
#include "vulkan_earth/ImageObject.h"
#include "vulkan_earth/MainMenuButton.h"
#include "vulkan_earth/PlayerFactory.h"
#include "vulkan_earth/PossibleGameStates.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/SubMenuEconomics.h"
#include "vulkan_earth/SubMenuHardware.h"
#include "vulkan_earth/SubMenuLandscape.h"
#include "vulkan_earth/SubMenuPhysics.h"
#include "vulkan_earth/SubMenuPlayOptions.h"
#include "vulkan_earth/SubMenuSelectTanks.h"
#include "vulkan_earth/SubMenuSound.h"
#include "vulkan_earth/SubMenuTest.h"
#include "vulkan_earth/SubMenuWeapons.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/MacroCrtdbg.h"

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

extern void playMusic(std::int32_t music);

MainMenu::MainMenu() = default;

MainMenu::MainMenu(float new_width,
    float new_height,
    float new_percent_border,
    GlobalSettings* new_global_settings,
    PlayerFactory* new_player_factory,
    std::int32_t* game_state) {
  m_global_settings = new_global_settings;
  m_player_factory = new_player_factory;
  m_current_game_state = game_state;

  for (std::int32_t x = 0; x < num_button; x++) {
    m_buttons[x] = nullptr;
  }
  for (std::int32_t x = 0; x < num_submenus; x++) {
    m_submenus[x] = nullptr;
  }
  for (std::int32_t x = 0; x < num_images; x++) {
    m_images[x] = nullptr;
  }
  for (std::int32_t x = 0; x < num_arrow_buttons; x++) {
    m_arrowsbutton[x] = nullptr;
  }
  m_button_pressed = nullptr;
  m_active_sub_menu = nullptr;
  m_arrow_button_pressed = nullptr;
  m_width = new_width;
  m_height = new_height;
  m_percent_border = new_percent_border;
  m_color[0] = m_color[1] = m_color[2] = 1;
  m_color[3] = 1;
  m_pos[0] = m_pos[1] = m_pos[2] = 0;

  /*	BUTTON 0 AND ITS SUBMENU	*/
  m_submenus[0] = new SubMenuSelectTanks(0,
      (-1 * m_width / 2.0 + .30 * m_width),
      ((m_height / 2.0) - .17 * m_height),
      3.0f,
      0.75f,
      0.75f,
      0.75f,
      0.65f * m_width,
      0.75 * m_height,
      "Select Your Tanks",
      .006);
  m_buttons[0] = new MainMenuButton(0,
      (-1 * m_width / 2.0 + .05 * m_width),
      ((m_height / 2.0) - .05 * m_height),
      1.0f,
      0.75f,
      0.0f,
      0.0f,
      0.20f * (m_width),
      0.04f * (m_height),
      "Start",
      m_submenus[0]);
  /*	ARROW BUTTON 1 NUMBER OF PLAYERS	*/
  m_arrowsbutton[0] =
      new ControlItemSelectionBox(-1 * m_width / 2.0 + .05 * m_width,
          ((m_height / 2.0) - .10 * m_height),
          1.0f,
          0.5f,
          0.5f,
          0.5f,
          0.2 * m_width,
          0.04 * (m_height),
          "# of Players",
          "2/3/4/5/6/7/8/9/10/");
  /*	ARROW BUTTON 2	NUMBER OF ROUNDS	*/
  m_arrowsbutton[1] =
      new ControlItemSelectionBox(-1 * m_width / 2.0 + .05 * m_width,
          ((m_height / 2.0) - .15 * m_height),
          1.0f,
          0.5f,
          0.5f,
          0.5f,
          0.2 * m_width,
          0.04 * (m_height),
          "# of Rounds",
          "1/2/3/4/5/6/7/8/9/");
  /*	BUTTON 1 AND ITS SUBMENU	*/
  m_submenus[1] = new SubMenuSound(1,
      (-1 * m_width / 2.0 + .30 * m_width),
      ((m_height / 2.0) - .17 * m_height),
      3.0f,
      0.75f,
      0.75f,
      0.75f,
      0.65f * m_width,
      0.75 * m_height,
      "Sound Options Menu",
      .006);
  m_buttons[1] = new MainMenuButton(1,
      (-1 * m_width / 2.0 + .05 * m_width),
      ((m_height / 2.0) - .20 * m_height),
      1.0f,
      0.75f,
      0.0f,
      0.0f,
      0.20f * (m_width),
      0.04f * (m_height),
      "Sound Options",
      m_submenus[1]);
  /*	BUTTON 2 AND ITS SUBMENU	*/
  m_submenus[2] = new SubMenuHardware(2,
      (-1 * m_width / 2.0 + .30 * m_width),
      ((m_height / 2.0) - .17 * m_height),
      3.0f,
      0.75f,
      0.75f,
      0.75f,
      0.65f * m_width,
      0.75 * m_height,
      "Hardware Options Menu",
      .006);
  m_buttons[2] = new MainMenuButton(2,
      (-1 * m_width / 2.0 + .05 * m_width),
      ((m_height / 2.0) - .25 * m_height),
      1.0f,
      0.75f,
      0.0f,
      0.0f,
      0.20f * (m_width),
      0.04f * (m_height),
      "Hardware Options",
      m_submenus[2]);
  /*	BUTTON 3 AND ITS SUBMENU	*/
  m_submenus[3] = new SubMenuEconomics(3,
      (-1 * m_width / 2.0 + .30 * m_width),
      ((m_height / 2.0) - .17 * m_height),
      3.0f,
      0.75f,
      0.75f,
      0.75f,
      0.65f * m_width,
      0.75 * m_height,
      "Economics Options Menu",
      .006);
  m_buttons[3] = new MainMenuButton(3,
      (-1 * m_width / 2.0 + .05 * m_width),
      ((m_height / 2.0) - .30 * m_height),
      1.0f,
      0.75f,
      0.0f,
      0.0f,
      0.20f * (m_width),
      0.04f * (m_height),
      "Economics",
      m_submenus[3]);
  /*	BUTTON 4 AND ITS SUBMENU	*/
  m_submenus[4] = new SubMenuPhysics(4,
      (-1 * m_width / 2.0 + .30 * m_width),
      ((m_height / 2.0) - .17 * m_height),
      3.0f,
      0.75f,
      0.75f,
      0.75f,
      0.65f * m_width,
      0.75 * m_height,
      "Physics Options Menu",
      .006);
  m_buttons[4] = new MainMenuButton(4,
      (-1 * m_width / 2.0 + .05 * m_width),
      ((m_height / 2.0) - .35 * m_height),
      1.0f,
      0.75f,
      0.0f,
      0.0f,
      0.20f * (m_width),
      0.04f * (m_height),
      "Physics",
      m_submenus[4]);
  /*	BUTTON 5 AND ITS SUBMENU	*/
  m_submenus[5] = new SubMenuLandscape(5,
      (-1 * m_width / 2.0 + .30 * m_width),
      ((m_height / 2.0) - .17 * m_height),
      3.0f,
      0.75f,
      0.75f,
      0.75f,
      0.65f * m_width,
      0.75 * m_height,
      "Landscape Options Menu",
      .006);
  m_buttons[5] = new MainMenuButton(5,
      (-1 * m_width / 2.0 + .05 * m_width),
      ((m_height / 2.0) - .40 * m_height),
      1.0f,
      0.75f,
      0.0f,
      0.0f,
      0.20f * (m_width),
      0.04f * (m_height),
      "Landscape",
      m_submenus[5]);
  /*	BUTTON 6 AND ITS SUBMENU	*/
  m_submenus[6] = new SubMenuPlayOptions(6,
      (-1 * m_width / 2.0 + .30 * m_width),
      ((m_height / 2.0) - .17 * m_height),
      3.0f,
      0.75f,
      0.75f,
      0.75f,
      0.65f * m_width,
      0.75 * m_height,
      "Play Settings Options Menu",
      .006);
  m_buttons[6] = new MainMenuButton(6,
      (-1 * m_width / 2.0 + .05 * m_width),
      ((m_height / 2.0) - .45 * m_height),
      1.0f,
      0.75f,
      0.0f,
      0.0f,
      0.20f * (m_width),
      0.04f * (m_height),
      "Play Settings",
      m_submenus[6]);
  /*	BUTTON 7 AND ITS SUBMENU	*/
  m_submenus[7] = new SubMenuWeapons(7,
      (-1 * m_width / 2.0 + .30 * m_width),
      ((m_height / 2.0) - .17 * m_height),
      3.0f,
      0.75f,
      0.75f,
      0.75f,
      0.65f * m_width,
      0.75 * m_height,
      "Weapons Option Menu",
      .006);
  m_buttons[7] = new MainMenuButton(7,
      (-1 * m_width / 2.0 + .05 * m_width),
      ((m_height / 2.0) - .50 * m_height),
      1.0f,
      0.75f,
      0.0f,
      0.0f,
      0.20f * (m_width),
      0.04f * (m_height),
      "Weapons",
      m_submenus[7]);
  /*	BUTTON 8 SAVE OPTIONS	*/
  m_buttons[8] = new MainMenuButton(8,
      (-1 * m_width / 2.0 + .05 * m_width),
      ((m_height / 2.0) - .55 * m_height),
      1.0f,
      0.75f,
      0.0f,
      0.0f,
      0.20f * (m_width),
      0.04f * (m_height),
      "Save Settings",
      nullptr);

  /*	QUIT BUTTON AND IMAGES	*/
  m_buttons[9] = new MainMenuButton(9,
      (-1 * m_width / 2.0 + .05 * m_width),
      ((m_height / 2.0) - .87 * m_height),
      1.0f,
      0.75f,
      0.0f,
      0.0f,
      0.20f * (m_width),
      0.04f * (m_height),
      "Quit",
      nullptr);
  m_images[0] = new ImageObject((-1 * m_width / 2.0 + .30 * m_width),
      ((m_height / 2.0) - .17 * m_height),
      2.0f,
      0.65f * m_width,
      0.75 * m_height,
      .006 * (m_width),
      1280,
      1024,
      "SplashScreen.raw");
  m_images[1] = new ImageObject((-1 * m_width / 2.0 + .30 * m_width),
      ((m_height / 2.0) - .03 * m_height),
      2.0f,
      0.65f * m_width,
      0.12 * m_height,
      .006 * (m_width),
      800,
      150,
      "vulkanEarthTitle.raw");
  new_global_settings->setCurrentTerrain(
      (static_cast<SubMenuLandscape*>(m_submenus[5]))->m_tm);
}

MainMenu::~MainMenu() {
  for (std::int32_t i = 0; i < num_images; i++) delete m_images[i];
  for (std::int32_t i = 0; i < num_button; i++) delete m_buttons[i];
  for (std::int32_t i = 0; i < num_submenus; i++) delete m_submenus[i];
  for (std::int32_t i = 0; i < num_arrow_buttons; i++)
    delete m_arrowsbutton[i];
}

float* MainMenu::getPos() { return &(m_pos[0]); }
float MainMenu::getHeight() { return m_width; }
void MainMenu::setHeight(float new_height) { m_height = new_height; }
float MainMenu::getWidth() { return m_height; }
void MainMenu::setWidth(float new_width) { m_width = new_width; }
float* MainMenu::getColor() { return &(m_color[0]); }
SubMenu* MainMenu::getSubMenuI(std::int32_t i) { return m_submenus[i]; }
SubMenu* MainMenu::getActiveSubMenu() { return m_active_sub_menu; }
SubMenuLandscape* MainMenu::getSubMenuLandscape() {
  return static_cast<SubMenuLandscape*>(m_submenus[5]);
}

void MainMenu::draw(render::RenderContext& context) {
  playMusic(mainmenu);
  // The whole-window background panel (see appendMenuPanel()).
  if (m_background_mesh.triangles().empty() || m_built_width != m_width ||
      m_built_height != m_height) {
    m_background_mesh.clear();
    vulkan_earth::appendMenuPanel(
        m_background_mesh, m_width, m_height, m_percent_border);
    m_built_width = m_width;
    m_built_height = m_height;
  }
  context.draw(m_background_mesh);

  for (std::int32_t i = 0; i < num_images; i++) {
    if (m_images[i]) {
      m_images[i]->draw(context);
    }
  }
  for (std::int32_t i = 0; i < num_arrow_buttons; i++) {
    if (m_arrowsbutton[i]) {
      m_arrowsbutton[i]->draw(context);
    }
  }
  for (std::int32_t x = 0; x < num_button; x++) {
    if (m_buttons[x]) {
      m_buttons[x]->draw(context);
    }
  }
}

void MainMenu::buttonTest(
    std::int32_t x, std::int32_t y, std::int32_t button_down) {
  if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
    for (std::int32_t button_i = 0; button_i < num_button;
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
      if (button_i < num_arrow_buttons) {
        if (m_arrowsbutton[button_i]) {  // IF YOU DID NOT CLICK A
                                         // BUTTON PERHAPS YOU CLICKED
                                         // A ARROW BUTTON???
          if ((x >= m_arrowsbutton[button_i]->getXPos()) &&
              (x <= (m_arrowsbutton[button_i]->getXPos() +
                        m_arrowsbutton[button_i]->getWidth())) &&
              (y <= m_arrowsbutton[button_i]->getYPos()) &&
              (y >= (m_arrowsbutton[button_i]->getYPos() -
                        m_arrowsbutton[button_i]->getHeight()))) {
            m_arrowsbutton[button_i]->mouseClickEvent(x,
                y,
                button_down,
                true);  // YOU PRESSED OVER A ARROWBUTTON
            m_arrow_button_pressed = m_arrowsbutton[button_i];
          }
        }
      }
    }
  } else if (!button_down) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                              // GOES UP
    if (m_button_pressed !=
        nullptr) {  // IF THE LEFT CLICK WAS VALID AND INSIDE A BUTTON
                    // CHECK TO SEE IF YOU ARE STILL OVER SAME BUTTON
      if ((x >= m_button_pressed->getXPos()) &&
          (x <=
              (m_button_pressed->getXPos() + m_button_pressed->getWidth())) &&
          (y <= m_button_pressed->getYPos()) &&
          (y >=
              (m_button_pressed->getYPos() - m_button_pressed->getHeight()))) {
        if (m_button_pressed->isActive()) {  // IF OVER SAME BUTTON MAKE SURE
                                             // IT WAS NOT ALREADY ACTIVATED
                                             // SOME BUTTONS HAVE EXTRA ACTIONS
                                             // LIKE SAVING DATA TO XML FILES
          if (m_button_pressed->getUNIQUEIDENTIFIER() == 0) {
            collectData();  // ONE OF THE BUTTONS PRESSED WAS
                            // START OR SAVE SETTINGS
            m_player_factory->setNumberofPlayers(
                m_global_settings->getPlayerCount());
            m_player_factory->initializePlayerDataBase();
            m_global_settings->setCurrentTerrain(getSubMenuLandscape()->m_tm);
          }
          if (m_button_pressed->getUNIQUEIDENTIFIER() == 8) {
            collectData();
            m_player_factory->setNumberofPlayers(
                m_global_settings->getPlayerCount());
          } else {
            m_button_pressed->deactivateSubMenu();  // IF SO DEACTIVATE
                                                    // SUBMENU ATTACHED TO
                                                    // BUTTON
            m_active_sub_menu = nullptr;        // GET RID OF ACTIVE SUBMENU
            m_button_pressed->depressButton();  // DEPRESS THE BUTTON
                                                // (CHANGE DRAWING MODE)
          }
        } else {  // IF BUTTON WAS NOT ALREADY ACTIVATED
                  // SOME BUTTONS HAVE EXTRA ACTIONS LIKE SAVING DATA
                  // TO XML FILES
          if (m_button_pressed->getUNIQUEIDENTIFIER() ==
              quit) {  // IF QUIT BUTTON PRESSED (QUIT DEFINED AT
                       // VERY TOP)
            m_button_pressed->depressButton();  // DEPRESS THE QUIT BUTTON
            *(m_current_game_state) = QUIT_GAME;
          }
          if (m_button_pressed->getUNIQUEIDENTIFIER() == 0) {
            collectData();  // ONE OF THE BUTTONS PRESSED WAS
                            // START OR SAVE SETTINGS
            m_player_factory->setNumberofPlayers(
                m_global_settings->getPlayerCount());
            m_player_factory->initializePlayerDataBase();
            m_global_settings->setCurrentTerrain(getSubMenuLandscape()->m_tm);
            Mix_HaltMusic();
            *m_current_game_state = READY_MENU;
          }
          if (m_button_pressed->getUNIQUEIDENTIFIER() == 8) {
            collectData();
          } else {
            for (std::int32_t i = 0; i < num_button;
                i++) {             // DEACTIVATE ALL OTHER MENUS
              if (m_buttons[i]) {  // SAFEGUARD AGAINST NULL
                                   // POINTER
                m_buttons[i]->deactivateSubMenu();
              }
            }
            if (m_button_pressed->getUNIQUEIDENTIFIER() != 0) {
              m_button_pressed->activateSubMenu();  // ACTIVATE THE
                                                    // SUBMENU ASSOCIATED
                                                    // WITH THE BUTTON
                                                    // PRESSED ABOVE
              m_active_sub_menu =
                  m_button_pressed->getSubMenu();  // SET THE ACTIVE
                                                   // SUBMENU
            }
          }
        }
        m_button_pressed->depressButton();  // WHENEVER YOU RELEASE MOUSEBUTTON
                                            // DEPRESS THE BUTTON YOU MIGHT
                                            // HAVE PUSHED
        m_button_pressed = nullptr;  // YOU HANDLED THE BUTTON NOW CLEAR IT
      } else {                       // IF YOU RELEASE OUTSIDE ALL BUTTONS
        m_button_pressed->depressButton();  // DEPRESS BUTTON YOU
                                            // MIGHT HAVE CLICKED
        m_button_pressed = nullptr;         // NO BUTTON REALLY CLICKED
                                            // (MUST REMAIN OVER BUTTON)

        // WTF, THIS CAUSED A BUG! WHY WAS IT THERE?!
        // activeSubMenu=NULL;
        // //GET RID OF ACTIVE SUBMENU JUST IN CASE (in case of what?)
      }
    } else if (m_arrow_button_pressed !=
        nullptr) {  // IF YOU MANAGED TO CLICK INSIDE AN ARROW
                    // BUTTON
                    // CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
      if ((x >= m_arrow_button_pressed->getXPos()) &&
          (x <= (m_arrow_button_pressed->getXPos() +
                    m_arrow_button_pressed->getWidth())) &&
          (y <= m_arrow_button_pressed->getYPos()) &&
          (y >= (m_arrow_button_pressed->getYPos() -
                    m_arrow_button_pressed->getHeight()))) {
        m_arrow_button_pressed->mouseClickEvent(x,
            y,
            button_down,
            true);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                    // RELEASE THE MOUSE
      } else {
        m_arrow_button_pressed->mouseClickEvent(x,
            y,
            button_down,
            false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                     // RELEASE THE MOUSE
        m_arrow_button_pressed = nullptr;
      }
    }
  }
  // Sub Menu Button Test
  if (m_active_sub_menu != nullptr) {
    m_active_sub_menu->subMenuMouseTest(x, y, button_down);
  }
}

void MainMenu::collectData() {
  std::string optionsarray;
  for (std::int32_t x = 2; x < num_submenus;
      x++) {  // SOUND AND START GAME ARE 1 AND 0 RESPECTIVLY
    if (m_submenus[x]) {
      optionsarray += m_submenus[x]->collectData();
    }
  }

  std::string playercount = "/Player Count/";
  if (m_arrowsbutton[0]) {
    playercount += m_arrowsbutton[0]->collectData();
    playercount += "/";
  }

  m_global_settings->setVariables(optionsarray, playercount);
}
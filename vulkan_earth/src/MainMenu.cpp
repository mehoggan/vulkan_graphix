#include "MainMenu.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "ControlItem.h"
#include "ControlItemCheckBox.h"
#include "ControlItemSelectionBox.h"
#include "GlobalSettings.h"
#include "ImageObject.h"
#include "MainMenuButton.h"
#include "PlayerFactory.h"
#include "SubMenu.h"
#include "SubMenuEconomics.h"
#include "SubMenuHardware.h"
#include "SubMenuLandscape.h"
#include "SubMenuPhysics.h"
#include "SubMenuPlayOptions.h"
#include "SubMenuSelectTanks.h"
#include "SubMenuSound.h"
#include "SubMenuTest.h"
#include "SubMenuWeapons.h"
#include "TextObject.h"
#include "macro_crtdbg.h"
#include "possibleGameStates.h"
#include "sound.h"

extern void playMusic(int music);

MainMenu::MainMenu() = default;

MainMenu::MainMenu(GLfloat new_width,
                   GLfloat new_height,
                   GLfloat new_percent_border,
                   GlobalSettings* new_global_settings,
                   PlayerFactory* new_player_factory,
                   int* game_state) {
    global_settings = new_global_settings;
    player_factory = new_player_factory;
    current_game_state = game_state;

    for (int x = 0; x < num_button; x++) {
        buttons[x] = nullptr;
    }
    for (int x = 0; x < num_submenus; x++) {
        submenus[x] = nullptr;
    }
    for (int x = 0; x < num_images; x++) {
        images[x] = nullptr;
    }
    for (int x = 0; x < num_arrow_buttons; x++) {
        arrowsbutton[x] = nullptr;
    }
    button_pressed = nullptr;
    active_sub_menu = nullptr;
    arrow_button_pressed = nullptr;
    width = new_width;
    height = new_height;
    percent_border = new_percent_border;
    color[0] = color[1] = color[2] = 1;
    color[3] = 1;
    pos[0] = pos[1] = pos[2] = 0;

    /*	BUTTON 0 AND ITS SUBMENU	*/
    submenus[0] = new SubMenuSelectTanks(0,
                                         (-1 * width / 2.0 + .30 * width),
                                         ((height / 2.0) - .17 * height),
                                         3.0f,
                                         0.75f,
                                         0.75f,
                                         0.75f,
                                         0.65f * width,
                                         0.75 * height,
                                         "Select Your Tanks",
                                         .006);
    buttons[0] = new MainMenuButton(0,
                                    (-1 * width / 2.0 + .05 * width),
                                    ((height / 2.0) - .05 * height),
                                    1.0f,
                                    0.75f,
                                    0.0f,
                                    0.0f,
                                    0.20f * (width),
                                    0.04f * (height),
                                    "Start",
                                    submenus[0]);
    /*	ARROW BUTTON 1 NUMBER OF PLAYERS	*/
    arrowsbutton[0] =
            new ControlItemSelectionBox(-1 * width / 2.0 + .05 * width,
                                        ((height / 2.0) - .10 * height),
                                        1.0f,
                                        0.5f,
                                        0.5f,
                                        0.5f,
                                        0.2 * width,
                                        0.04 * (height),
                                        "# of Players",
                                        "2/3/4/5/6/7/8/9/10/");
    /*	ARROW BUTTON 2	NUMBER OF ROUNDS	*/
    arrowsbutton[1] =
            new ControlItemSelectionBox(-1 * width / 2.0 + .05 * width,
                                        ((height / 2.0) - .15 * height),
                                        1.0f,
                                        0.5f,
                                        0.5f,
                                        0.5f,
                                        0.2 * width,
                                        0.04 * (height),
                                        "# of Rounds",
                                        "1/2/3/4/5/6/7/8/9/");
    /*	BUTTON 1 AND ITS SUBMENU	*/
    submenus[1] = new SubMenuSound(1,
                                   (-1 * width / 2.0 + .30 * width),
                                   ((height / 2.0) - .17 * height),
                                   3.0f,
                                   0.75f,
                                   0.75f,
                                   0.75f,
                                   0.65f * width,
                                   0.75 * height,
                                   "Sound Options Menu",
                                   .006);
    buttons[1] = new MainMenuButton(1,
                                    (-1 * width / 2.0 + .05 * width),
                                    ((height / 2.0) - .20 * height),
                                    1.0f,
                                    0.75f,
                                    0.0f,
                                    0.0f,
                                    0.20f * (width),
                                    0.04f * (height),
                                    "Sound Options",
                                    submenus[1]);
    /*	BUTTON 2 AND ITS SUBMENU	*/
    submenus[2] = new SubMenuHardware(2,
                                      (-1 * width / 2.0 + .30 * width),
                                      ((height / 2.0) - .17 * height),
                                      3.0f,
                                      0.75f,
                                      0.75f,
                                      0.75f,
                                      0.65f * width,
                                      0.75 * height,
                                      "Hardware Options Menu",
                                      .006);
    buttons[2] = new MainMenuButton(2,
                                    (-1 * width / 2.0 + .05 * width),
                                    ((height / 2.0) - .25 * height),
                                    1.0f,
                                    0.75f,
                                    0.0f,
                                    0.0f,
                                    0.20f * (width),
                                    0.04f * (height),
                                    "Hardware Options",
                                    submenus[2]);
    /*	BUTTON 3 AND ITS SUBMENU	*/
    submenus[3] = new SubMenuEconomics(3,
                                       (-1 * width / 2.0 + .30 * width),
                                       ((height / 2.0) - .17 * height),
                                       3.0f,
                                       0.75f,
                                       0.75f,
                                       0.75f,
                                       0.65f * width,
                                       0.75 * height,
                                       "Economics Options Menu",
                                       .006);
    buttons[3] = new MainMenuButton(3,
                                    (-1 * width / 2.0 + .05 * width),
                                    ((height / 2.0) - .30 * height),
                                    1.0f,
                                    0.75f,
                                    0.0f,
                                    0.0f,
                                    0.20f * (width),
                                    0.04f * (height),
                                    "Economics",
                                    submenus[3]);
    /*	BUTTON 4 AND ITS SUBMENU	*/
    submenus[4] = new SubMenuPhysics(4,
                                     (-1 * width / 2.0 + .30 * width),
                                     ((height / 2.0) - .17 * height),
                                     3.0f,
                                     0.75f,
                                     0.75f,
                                     0.75f,
                                     0.65f * width,
                                     0.75 * height,
                                     "Physics Options Menu",
                                     .006);
    buttons[4] = new MainMenuButton(4,
                                    (-1 * width / 2.0 + .05 * width),
                                    ((height / 2.0) - .35 * height),
                                    1.0f,
                                    0.75f,
                                    0.0f,
                                    0.0f,
                                    0.20f * (width),
                                    0.04f * (height),
                                    "Physics",
                                    submenus[4]);
    /*	BUTTON 5 AND ITS SUBMENU	*/
    submenus[5] = new SubMenuLandscape(5,
                                       (-1 * width / 2.0 + .30 * width),
                                       ((height / 2.0) - .17 * height),
                                       3.0f,
                                       0.75f,
                                       0.75f,
                                       0.75f,
                                       0.65f * width,
                                       0.75 * height,
                                       "Landscape Options Menu",
                                       .006);
    buttons[5] = new MainMenuButton(5,
                                    (-1 * width / 2.0 + .05 * width),
                                    ((height / 2.0) - .40 * height),
                                    1.0f,
                                    0.75f,
                                    0.0f,
                                    0.0f,
                                    0.20f * (width),
                                    0.04f * (height),
                                    "Landscape",
                                    submenus[5]);
    /*	BUTTON 6 AND ITS SUBMENU	*/
    submenus[6] = new SubMenuPlayOptions(6,
                                         (-1 * width / 2.0 + .30 * width),
                                         ((height / 2.0) - .17 * height),
                                         3.0f,
                                         0.75f,
                                         0.75f,
                                         0.75f,
                                         0.65f * width,
                                         0.75 * height,
                                         "Play Settings Options Menu",
                                         .006);
    buttons[6] = new MainMenuButton(6,
                                    (-1 * width / 2.0 + .05 * width),
                                    ((height / 2.0) - .45 * height),
                                    1.0f,
                                    0.75f,
                                    0.0f,
                                    0.0f,
                                    0.20f * (width),
                                    0.04f * (height),
                                    "Play Settings",
                                    submenus[6]);
    /*	BUTTON 7 AND ITS SUBMENU	*/
    submenus[7] = new SubMenuWeapons(7,
                                     (-1 * width / 2.0 + .30 * width),
                                     ((height / 2.0) - .17 * height),
                                     3.0f,
                                     0.75f,
                                     0.75f,
                                     0.75f,
                                     0.65f * width,
                                     0.75 * height,
                                     "Weapons Option Menu",
                                     .006);
    buttons[7] = new MainMenuButton(7,
                                    (-1 * width / 2.0 + .05 * width),
                                    ((height / 2.0) - .50 * height),
                                    1.0f,
                                    0.75f,
                                    0.0f,
                                    0.0f,
                                    0.20f * (width),
                                    0.04f * (height),
                                    "Weapons",
                                    submenus[7]);
    /*	BUTTON 8 SAVE OPTIONS	*/
    buttons[8] = new MainMenuButton(8,
                                    (-1 * width / 2.0 + .05 * width),
                                    ((height / 2.0) - .55 * height),
                                    1.0f,
                                    0.75f,
                                    0.0f,
                                    0.0f,
                                    0.20f * (width),
                                    0.04f * (height),
                                    "Save Settings",
                                    nullptr);

    /*	QUIT BUTTON AND IMAGES	*/
    buttons[9] = new MainMenuButton(9,
                                    (-1 * width / 2.0 + .05 * width),
                                    ((height / 2.0) - .87 * height),
                                    1.0f,
                                    0.75f,
                                    0.0f,
                                    0.0f,
                                    0.20f * (width),
                                    0.04f * (height),
                                    "Quit",
                                    nullptr);
    images[0] = new ImageObject((-1 * width / 2.0 + .30 * width),
                                ((height / 2.0) - .17 * height),
                                2.0f,
                                0.65f * width,
                                0.75 * height,
                                .006 * (width),
                                1280,
                                1024,
                                "SplashScreen.raw");
    images[1] = new ImageObject((-1 * width / 2.0 + .30 * width),
                                ((height / 2.0) - .03 * height),
                                2.0f,
                                0.65f * width,
                                0.12 * height,
                                .006 * (width),
                                800,
                                150,
                                "vulkanEarthTitle.raw");
    new_global_settings->setCurrentTerrain(
            (static_cast<SubMenuLandscape*>(submenus[5]))->tm);
}

MainMenu::~MainMenu() {
    for (int i = 0; i < num_images; i++) delete images[i];
    for (int i = 0; i < num_button; i++) delete buttons[i];
    for (int i = 0; i < num_submenus; i++) delete submenus[i];
    for (int i = 0; i < num_arrow_buttons; i++) delete arrowsbutton[i];
}

GLfloat* MainMenu::getPos() { return &(pos[0]); }
GLfloat MainMenu::getHeight() { return width; }
void MainMenu::setHeight(GLfloat new_height) { height = new_height; }
GLfloat MainMenu::getWidth() { return height; }
void MainMenu::setWidth(GLfloat new_width) { width = new_width; }
GLfloat* MainMenu::getColor() { return &(color[0]); }
SubMenu* MainMenu::getSubMenuI(int i) { return submenus[i]; }
SubMenu* MainMenu::getActiveSubMenu() { return active_sub_menu; }
SubMenuLandscape* MainMenu::getSubMenuLandscape() {
    return static_cast<SubMenuLandscape*>(submenus[5]);
}

void MainMenu::draw() {
    playMusic(mainmenu);
    glPushMatrix();
    /*	ALWAYS START AT UPPER LEFT CORNER -> LOWER LEFT CORNER -> LOWER RIGHT
     * CORNER -> UPPER RIGHT	*/
    glBegin(GL_QUADS);
    glColor3f(0.85f, 0.85f, 0.85f);
    glVertex3f(-1 * (width / 2.0), (height / 2.0), 0);      /*	|\ 	*/
    glVertex3f(-1 * (width / 2.0), -1 * (height / 2.0), 0); /*	| |	*/
    glVertex3f(-1 * (width / 2.0) + (percent_border * (height)),
               -1 * (height / 2.0) + (percent_border * (height)),
               0); /*	|/ 	*/
    glVertex3f(-1 * (width / 2.0) + (percent_border * (height)),
               (height / 2.0) - (percent_border * (height)),
               0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.80f, 0.80f, 0.80f);
    glVertex3f(-1 * (width / 2.0), (height / 2.0), 0); /*_____ */
    glVertex3f(-1 * (width / 2.0) + (percent_border * (height)),
               (height / 2.0) - (percent_border * (height)),
               0); /*\	  / */
    glVertex3f((width / 2.0) - (percent_border * (height)),
               (height / 2.0) - (percent_border * (height)),
               0); /* ---	*/
    glVertex3f((width / 2.0), (height / 2.0), 0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.75f, 0.75f, 0.75f);
    glVertex3f(-1 * (width / 2.0) + (percent_border * (height)),
               (height / 2.0) - (percent_border * (height)),
               0); /*_____ */
    glVertex3f(-1 * (width / 2.0) + (percent_border * (height)),
               -1 * (height / 2.0) + (percent_border * (height)),
               0); /*|	  | */
    glVertex3f((width / 2.0) - (percent_border * (height)),
               -1 * (height / 2.0) + (percent_border * (height)),
               0); /*----- */
    glVertex3f((width / 2.0) - (percent_border * (height)),
               (height / 2.0) - (percent_border * (height)),
               0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.45f, 0.45f, 0.45f);
    glVertex3f(-1 * (width / 2.0) + (percent_border * (height)),
               -1 * (height / 2.0) + (percent_border * (height)),
               0);                                          /* ___  */
    glVertex3f(-1 * (width / 2.0), -1 * (height / 2.0), 0); /*/	  \ */
    glVertex3f((width / 2.0), -1 * (height / 2.0), 0);      /*----- */
    glVertex3f((width / 2.0) - (percent_border * (height)),
               -1 * (height / 2.0) + (percent_border * (height)),
               0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.40f, 0.40f, 0.40f);
    glVertex3f((width / 2.0) - (percent_border * (height)),
               (height / 2.0) - (percent_border * (height)),
               0); /*	 /| 	*/
    glVertex3f((width / 2.0) - (percent_border * (height)),
               -1 * (height / 2.0) + (percent_border * (height)),
               0);                                     /*	| | 	*/
    glVertex3f((width / 2.0), -1 * (height / 2.0), 0); /* 	 \| 	*/
    glVertex3f((width / 2.0), (height / 2.0), 0);
    glEnd();

    for (int i = 0; i < num_images; i++) {
        if (images[i]) {
            images[i]->draw();
        }
    }
    for (int i = 0; i < num_arrow_buttons; i++) {
        if (arrowsbutton[i]) {
            arrowsbutton[i]->draw();
        }
    }
    for (int x = 0; x < num_button; x++) {
        if (buttons[x]) {
            buttons[x]->draw();
        }
    }

    glPopMatrix();
}

void MainMenu::buttonTest(int x, int y, int button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < num_button;
             button_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                            // IF CLICK LANDS IN BUTTON I
            if (buttons[button_i]) {  // JUST TO MAKE SURE
                if ((x >= buttons[button_i]->getXPos()) &&
                    (x <= (buttons[button_i]->getXPos() +
                           buttons[button_i]->getWidth())) &&
                    (y <= buttons[button_i]->getYPos()) &&
                    (y >= (buttons[button_i]->getYPos() -
                           buttons[button_i]->getHeight()))) {
                    buttons[button_i]->pressButton();    // PRESS BUTTON
                    button_pressed = buttons[button_i];  // KEEP TRACK OF WHICH
                                                         // BUTTON WAS PRESSED
                }
            }
            if (button_i < num_arrow_buttons) {
                if (arrowsbutton[button_i]) {  // IF YOU DID NOT CLICK A BUTTON
                                               // PERHAPS YOU CLICKED A ARROW
                                               // BUTTON???
                    if ((x >= arrowsbutton[button_i]->getXPos()) &&
                        (x <= (arrowsbutton[button_i]->getXPos() +
                               arrowsbutton[button_i]->getWidth())) &&
                        (y <= arrowsbutton[button_i]->getYPos()) &&
                        (y >= (arrowsbutton[button_i]->getYPos() -
                               arrowsbutton[button_i]->getHeight()))) {
                        arrowsbutton[button_i]->mouseClickEvent(
                                x,
                                y,
                                button_down,
                                true);  // YOU PRESSED OVER A ARROWBUTTON
                        arrow_button_pressed = arrowsbutton[button_i];
                    }
                }
            }
        }
    } else if (!button_down) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                                // GOES UP
        if (button_pressed !=
            nullptr) {  // IF THE LEFT CLICK WAS VALID AND INSIDE A BUTTON
                        // CHECK TO SEE IF YOU ARE STILL OVER SAME BUTTON
            if ((x >= button_pressed->getXPos()) &&
                (x <=
                 (button_pressed->getXPos() + button_pressed->getWidth())) &&
                (y <= button_pressed->getYPos()) &&
                (y >=
                 (button_pressed->getYPos() - button_pressed->getHeight()))) {
                if (button_pressed
                            ->isActive()) {  // IF OVER SAME BUTTON MAKE SURE
                                             // IT WAS NOT ALREADY ACTIVATED
                                             // SOME BUTTONS HAVE EXTRA ACTIONS
                                             // LIKE SAVING DATA TO XML FILES
                    if (button_pressed->getUNIQUEIDENTIFIER() == 0) {
                        collectData();  // ONE OF THE BUTTONS PRESSED WAS
                                        // START OR SAVE SETTINGS
                        player_factory->setNumberofPlayers(
                                global_settings->getPlayerCount());
                        player_factory->initializePlayerDataBase();
                        global_settings->setCurrentTerrain(
                                getSubMenuLandscape()->tm);
                    }
                    if (button_pressed->getUNIQUEIDENTIFIER() == 8) {
                        collectData();
                        player_factory->setNumberofPlayers(
                                global_settings->getPlayerCount());
                    } else {
                        button_pressed
                                ->deactivateSubMenu();  // IF SO DEACTIVATE
                                                        // SUBMENU ATTACHED TO
                                                        // BUTTON
                        active_sub_menu =
                                nullptr;  // GET RID OF ACTIVE SUBMENU
                        button_pressed
                                ->depressButton();  // DEPRESS THE BUTTON
                                                    // (CHANGE DRAWING MODE)
                    }
                } else {  // IF BUTTON WAS NOT ALREADY ACTIVATED
                          // SOME BUTTONS HAVE EXTRA ACTIONS LIKE SAVING DATA
                          // TO XML FILES
                    if (button_pressed->getUNIQUEIDENTIFIER() ==
                        quit) {  // IF QUIT BUTTON PRESSED (QUIT DEFINED AT
                                 // VERY TOP)
                        button_pressed
                                ->depressButton();  // DEPRESS THE QUIT BUTTON
                        *(current_game_state) = QUIT_GAME;
                    }
                    if (button_pressed->getUNIQUEIDENTIFIER() == 0) {
                        collectData();  // ONE OF THE BUTTONS PRESSED WAS
                                        // START OR SAVE SETTINGS
                        player_factory->setNumberofPlayers(
                                global_settings->getPlayerCount());
                        player_factory->initializePlayerDataBase();
                        global_settings->setCurrentTerrain(
                                getSubMenuLandscape()->tm);
                        Mix_HaltMusic();
                        *current_game_state = READY_MENU;
                    }
                    if (button_pressed->getUNIQUEIDENTIFIER() == 8) {
                        collectData();
                    } else {
                        for (int i = 0; i < num_button;
                             i++) {            // DEACTIVATE ALL OTHER MENUS
                            if (buttons[i]) {  // SAFEGUARD AGAINST NULL
                                               // POINTER
                                buttons[i]->deactivateSubMenu();
                            }
                        }
                        if (button_pressed->getUNIQUEIDENTIFIER() != 0) {
                            button_pressed
                                    ->activateSubMenu();  // ACTIVATE THE
                                                          // SUBMENU ASSOCIATED
                                                          // WITH THE BUTTON
                                                          // PRESSED ABOVE
                            active_sub_menu =
                                    button_pressed
                                            ->getSubMenu();  // SET THE ACTIVE
                                                             // SUBMENU
                        }
                    }
                }
                button_pressed
                        ->depressButton();  // WHENEVER YOU RELEASE MOUSEBUTTON
                                            // DEPRESS THE BUTTON YOU MIGHT
                                            // HAVE PUSHED
                button_pressed =
                        nullptr;  // YOU HANDLED THE BUTTON NOW CLEAR IT
            } else {              // IF YOU RELEASE OUTSIDE ALL BUTTONS
                button_pressed->depressButton();  // DEPRESS BUTTON YOU
                                                  // MIGHT HAVE CLICKED
                button_pressed = nullptr;         // NO BUTTON REALLY CLICKED
                                                  // (MUST REMAIN OVER BUTTON)

                // WTF, THIS CAUSED A BUG! WHY WAS IT THERE?!
                // activeSubMenu=NULL;
                // //GET RID OF ACTIVE SUBMENU JUST IN CASE (in case of what?)
            }
        } else if (arrow_button_pressed !=
                   nullptr) {  // IF YOU MANAGED TO CLICK INSIDE AN ARROW
                               // BUTTON
                               // CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
            if ((x >= arrow_button_pressed->getXPos()) &&
                (x <= (arrow_button_pressed->getXPos() +
                       arrow_button_pressed->getWidth())) &&
                (y <= arrow_button_pressed->getYPos()) &&
                (y >= (arrow_button_pressed->getYPos() -
                       arrow_button_pressed->getHeight()))) {
                arrow_button_pressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        true);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                // RELEASE THE MOUSE
            } else {
                arrow_button_pressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                 // RELEASE THE MOUSE
                arrow_button_pressed = nullptr;
            }
        }
    }
    // Sub Menu Button Test
    if (active_sub_menu != nullptr) {
        active_sub_menu->subMenuMouseTest(x, y, button_down);
    }
}

void MainMenu::collectData() {
    std::string optionsarray;
    for (int x = 2; x < num_submenus;
         x++) {  // SOUND AND START GAME ARE 1 AND 0 RESPECTIVLY
        if (submenus[x]) {
            optionsarray += submenus[x]->collectData();
        }
    }

    std::string playercount = "/Player Count/";
    if (arrowsbutton[0]) {
        playercount += arrowsbutton[0]->collectData();
        playercount += "/";
    }

    global_settings->setVariables(optionsarray, playercount);
}
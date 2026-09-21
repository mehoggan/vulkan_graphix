// Vulkan Earth
//<Insert Team Name Here>
//
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <iostream>
#include <new>
#include "GameState.h"
#include "GlobalSettings.h"
#include "LoadingScreen.h"
#include "MainMenu.h"
#include "PlayerFactory.h"
#include "ReadyMenu.h"
#include "ShopMenu.h"
#include "SubMenu.h"
#include "SubMenuLandscape.h"
#include "TerrainMaker.h"
#include "macro_crtdbg.h"
#include "math.h"
#include "possibleGameStates.h"

#ifdef new
#undef new
#endif

using namespace std;

void resize(int, int);
void draw();
void timerEvent(int);
void keyHandler(unsigned char, int, int);
void keyHandlerUp(unsigned char key, int x, int y);
void specKeyHandler(int key, int x, int y);
void specKeyHandlerUp(int key, int x, int y);
void mouseHandler(int, int, int, int);
void mouseMotionHandler(int, int);
extern void initSound();
void initStuff();

int screen_state;
int prev_screen_state;
enum removeGlutStates { UP, DOWN };
int win_width, win_height;
int timer;
MainMenu* mainmenu;
ReadyMenu* readymenu;
ShopMenu* shopmenu;
GlobalSettings* global_settings;
PlayerFactory* player_factory;
GameState* game_state;
LoadingScreen* loading_screen;

// GLUT_SCREEN_WIDTH/HEIGHT is the whole X11 screen - the combined
// virtual desktop spanning every monitor, not just the primary one
// (unlike Windows, where it's the primary display's resolution). Ask
// RandR for the primary monitor's real geometry instead, so the window
// created from it stays confined to that one monitor.
static void primaryMonitorGeometry(int* pos_x,
                                   int* pos_y,
                                   int* width,
                                   int* height) {
    *pos_x = 0;
    *pos_y = 0;
    *width = 1280;
    *height = 800;
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        return;
    }
    Window root = RootWindow(display, DefaultScreen(display));
    XRRScreenResources* resources = XRRGetScreenResources(display, root);
    if (resources) {
        RROutput primary = XRRGetOutputPrimary(display, root);
        XRROutputInfo* output =
                primary ? XRRGetOutputInfo(display, resources, primary)
                        : nullptr;
        if (output && output->crtc) {
            XRRCrtcInfo* crtc =
                    XRRGetCrtcInfo(display, resources, output->crtc);
            if (crtc) {
                *pos_x = crtc->x;
                *pos_y = crtc->y;
                *width = crtc->width;
                *height = crtc->height;
                XRRFreeCrtcInfo(crtc);
            }
        }
        if (output) {
            XRRFreeOutputInfo(output);
        }
        XRRFreeScreenResources(resources);
    }
    XCloseDisplay(display);
}

int main(int argc, char* argv[]) {
    screen_state = MAIN_MENU;
    prev_screen_state = screen_state;

    initSound();
    glutInit(&argc, argv);
    int win_pos_x, win_pos_y;
    primaryMonitorGeometry(&win_pos_x, &win_pos_y, &win_width, &win_height);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(win_pos_x, win_pos_y);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow("VulkanEarth");
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    glewInit();

    glutReshapeFunc(resize);
    glutDisplayFunc(draw);
    glutTimerFunc(20, timerEvent, 1);
    glutKeyboardFunc(keyHandler);
    glutKeyboardUpFunc(keyHandlerUp);
    glutSpecialFunc(specKeyHandler);
    glutSpecialUpFunc(specKeyHandlerUp);
    glutMouseFunc(mouseHandler);
    glutMotionFunc(mouseMotionHandler);
    screen_state = MAIN_MENU;

    global_settings = new GlobalSettings();
    player_factory = new PlayerFactory(global_settings);
    player_factory->setNumberofPlayers(global_settings->getPlayerCount());
    mainmenu = new MainMenu(win_width,
                            win_height,
                            0.01f,
                            global_settings,
                            player_factory,
                            &screen_state);
    loading_screen = new LoadingScreen(-win_width / 4.0,
                                       win_height / 4.0,
                                       10,
                                       win_width * 0.5,
                                       win_height * 0.5,
                                       0.75,
                                       0.75,
                                       0.75,
                                       1);
    game_state = nullptr;
    readymenu = nullptr;
    shopmenu = nullptr;

    initStuff();
    glutMainLoop();
    delete mainmenu;
    delete readymenu;
    delete shopmenu;
    delete global_settings;
    delete player_factory;
    delete game_state;
    delete loading_screen;
    return 0;
}

void resize(int width, int height) {
    win_width = glutGet(GLUT_WINDOW_WIDTH);
    win_height = glutGet(GLUT_WINDOW_HEIGHT);
    glViewport(0, 0, win_width, win_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
            60.0,
            static_cast<float>(win_width) / static_cast<float>(win_height),
            1.0,
            1000000.0);
    glMatrixMode(GL_MODELVIEW);
    win_width = width;
    win_height = height;
    mainmenu->setWidth(width);
    mainmenu->setHeight(height);
    if (readymenu) {
        readymenu->setWidth(width);
        readymenu->setHeight(height);
    }
    if (shopmenu) {
    }
}

void timerEvent(int msec) {
    glutTimerFunc(msec, timerEvent, 1);
    draw();
}

void draw() {
    glClearColor(0, 0, 0, 0);  // background color
    glClearDepth(1.0f);        // 0 is near, 1 is far
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    if (screen_state != GAME_PLAY) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glViewport(0, 0, win_width, win_height);
        gluPerspective(
                60.0,
                static_cast<float>(win_width) / static_cast<float>(win_height),
                1.0,
                1000000.0);

        glMatrixMode(GL_MODELVIEW);
        glScissor(0, 0, win_width, win_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
    } else {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glViewport(0, 0, win_width, win_height);
        gluPerspective(
                60.0,
                static_cast<float>(win_width) / static_cast<float>(win_height),
                100.0,
                100000000.0);

        glMatrixMode(GL_MODELVIEW);
        glScissor(0, 0, win_width, win_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
    }
    switch (screen_state) {
        case MAIN_MENU: {
            if (prev_screen_state != MAIN_MENU) {
                mainmenu->getSubMenuLandscape()->tm->prepareData(
                        0, 0, 0, 0, 0);
                glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            }
            int distance = win_height / 2 * tan(1.04719755);
            gluLookAt(0, 0, distance, 0, 0, 0, 0, 1, 0);
            mainmenu->draw();
            break;
        }
        case READY_MENU: {
            if (readymenu == nullptr) {
                int distance = win_height / 2 * tan(1.04719755);
                gluLookAt(0, 0, distance, 0, 0, 0, 0, 1, 0);
                loading_screen->draw();
                glutSwapBuffers();
                readymenu = new ReadyMenu(win_width,
                                          win_height,
                                          0.01f,
                                          global_settings,
                                          player_factory,
                                          &screen_state);
            }
            if (prev_screen_state != READY_MENU) {
                readymenu->updatePageInfo();
            }
            int distance = win_height / 2 * tan(1.04719755);
            gluLookAt(0, 0, distance, 0, 0, 0, 0, 1, 0);
            readymenu->draw();
            break;
        }
        case SHOP_MENU: {
            if (shopmenu == nullptr) {
                shopmenu = new ShopMenu(win_width,
                                        win_height,
                                        0.01f,
                                        global_settings,
                                        player_factory,
                                        &screen_state);
            }
            if (game_state) {
                delete game_state;
                game_state = nullptr;
            }
            int distance = win_height / 2 * tan(1.04719755);
            gluLookAt(0, 0, distance, 0, 0, 0, 0, 1, 0);
            if (screen_state == SHOP_MENU) shopmenu->draw();
            break;
        }
        case GAME_PLAY: {
            if (game_state == nullptr) {
                game_state = new GameState(win_width,
                                           win_height,
                                           player_factory,
                                           global_settings,
                                           &screen_state);
                glutSetCursor(GLUT_CURSOR_NONE);
            }
            if (readymenu) {
                delete readymenu;
                readymenu = nullptr;
            }
            if (shopmenu) {
                delete shopmenu;
                shopmenu = nullptr;
            }
            int distance = win_height / 2 * tan(1.04719755);
            game_state->draw();

            glPushMatrix();
            glBegin(GL_LINES);
            glColor3f(1, 0, 0);
            glVertex3f(0, 0, 0);
            glVertex3f(1000, 0, 0);
            glColor3f(0, 1, 0);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 1000, 0);
            glColor3f(0, 0, 1);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 0, 1000);
            glColor3f(1, 1, 1);
            glEnd();
            glPopMatrix();

            break;
        }
        case QUIT_GAME: {
            delete mainmenu;
            delete readymenu;
            delete shopmenu;
            delete game_state;
            delete global_settings;
            delete player_factory;
            exit(0);  // this is just temporary, delete this later
        }
        default: {
            cout << "Error" << endl;
        }
    }

    if (screen_state != prev_screen_state) {
        if (readymenu)
            readymenu->updateNumPlayers(global_settings->getPlayerCount());
        if (shopmenu)
            shopmenu->updateNumPlayers(global_settings->getPlayerCount());
        prev_screen_state = screen_state;
    }

    glutSwapBuffers();
}

void keyHandler(unsigned char key, int x, int y) {
    if (screen_state == READY_MENU) {
        if (key == 27) {
            screen_state = QUIT_GAME;
        }
        if (readymenu) readymenu->keyTest(key);
    }
    if (screen_state == SHOP_MENU) {
        if (key == 27) {
            delete mainmenu;
            delete readymenu;
            delete shopmenu;
            delete game_state;
            delete global_settings;
            delete player_factory;
            exit(0);  // this is just temporary, delete this later
        }
    }
    if (screen_state == MAIN_MENU) {
        switch (key) {
            case 27: {  // ESCAPE KEY
                delete mainmenu;
                delete readymenu;
                delete shopmenu;
                delete game_state;
                delete global_settings;
                delete player_factory;
                exit(0);
                break;
            }
        }
    }
    if (screen_state == GAME_PLAY) {
        if (key == 27) {
            delete mainmenu;
            delete readymenu;
            delete shopmenu;
            // delete game_state;			//UNCOMMENT ON RELEASE
            delete global_settings;
            delete player_factory;
            exit(0);
        } else if (key == 'n')
            global_settings->getCurrentTerrain()->toggleWireframe();
        else if (key == 'm')
            screen_state =
                    MAIN_MENU;  // this is just temporary, delete this later
        else
            game_state->handleKeyboardInput(key, true);
    }
}

void keyHandlerUp(unsigned char key, int x, int y) {
    if (screen_state == GAME_PLAY) {
        if (key == static_cast<int>('l')) {
            global_settings->getCurrentTerrain()->toggleWireframe();
        } else {
            game_state->handleKeyboardInput(key, false);
        }
    }
}

void specKeyHandler(int key, int x, int y) {
    if (screen_state == GAME_PLAY) {
        if (key == 100) {
            game_state->handleKeyboardInput(1, true);
        }
        if (key == 101) {
            game_state->handleKeyboardInput(2, true);
        }
        if (key == 102) {
            game_state->handleKeyboardInput(3, true);
        }
        if (key == 103) {
            game_state->handleKeyboardInput(4, true);
        }
        if (key == 1) {  // F1
            game_state->handleKeyboardInput(5, true);
        }
        if (key == 7) {  // F7
            game_state->handleKeyboardInput(6, true);
        }
        if (key == 2 || key == 3 || key == 4 || key == 5 || key == 6 ||
            key == 8 || key == 9 || key == 10 || key == 11) {
            game_state->handleKeyboardInput(254, true);
        }
    }
}

void specKeyHandlerUp(int key, int x, int y) {
    if (screen_state == GAME_PLAY) {
        if (key == 100) {
            game_state->handleKeyboardInput(1, false);
        }
        if (key == 101) {
            game_state->handleKeyboardInput(2, false);
        }
        if (key == 102) {
            game_state->handleKeyboardInput(3, false);
        }
        if (key == 103) {
            game_state->handleKeyboardInput(4, false);
        }
        if (key == 1) {  // F1
            game_state->handleKeyboardInput(5, false);
        }
        if (key == 7) {  // F7
            game_state->handleKeyboardInput(6, false);
        }
    }
}

void mouseHandler(int button, int state, int x, int y) {
    switch (screen_state) {
        case MAIN_MENU: {
            if (button == GLUT_LEFT_BUTTON) {
                if (state == GLUT_UP) {
                    state = 0;
                    // printf("The caption values in
                    // GLUT_MOUSEHANDLER_CALLBACK_MOUSEUP are now [0]=%s --
                    // [1]=%s\n",mainmenu->getSubMenuI(0)->getCaption(),mainmenu->getSubMenuI(1)->getCaption());
                } else {
                    state = 1;
                    // printf("The caption values in
                    // GLUT_MOUSEHANDLER_CALLBACK_MOUSEDOWN are now [0]=%s --
                    // [1]=%s\n",mainmenu->getSubMenuI(0)->getCaption(),mainmenu->getSubMenuI(1)->getCaption());
                }
                mainmenu->buttonTest(x - (win_width / 2),
                                     (win_height / 2) - y,
                                     state);  // remember to check about screen
                                              // size changing
            }
            break;
        }
        case READY_MENU: {
            if (button == GLUT_LEFT_BUTTON) {
                if (state == GLUT_UP) {
                    state = 0;
                } else {
                    state = 1;
                }
                if (readymenu)
                    readymenu->buttonTest(x - (win_width / 2),
                                          (win_height / 2) - y,
                                          state);  // remember to check about
                                                   // screen size changing
            }
            break;
        }
        case SHOP_MENU: {
            if (button == GLUT_LEFT_BUTTON) {
                if (state == GLUT_UP) {
                    state = 0;
                } else {
                    state = 1;
                }
                if (shopmenu)
                    shopmenu->buttonTest(x - (win_width / 2),
                                         (win_height / 2) - y,
                                         state);  // remember to check about
                                                  // screen size changing
            }
            break;
        }
        case GAME_PLAY: {
            break;
        }
        default: {
            cout << "Error" << endl;
        }
    }
}

void mouseMotionHandler(int x, int y) {
    if (mainmenu->getActiveSubMenu() != nullptr) {
        if (mainmenu->getActiveSubMenu()->getUNIQUEIDENTIFIER() == 1) {
            mainmenu->getActiveSubMenu()->updateMouse(x - (win_width / 2),
                                                      (win_height / 2) - y);
        } else if (mainmenu->getActiveSubMenu()->getUNIQUEIDENTIFIER() == 5) {
            mainmenu->getActiveSubMenu()->updateMouse(x, y);
        }
    }
    if (screen_state == READY_MENU) {
        if (readymenu)
            readymenu->updateMouse(x - (win_width / 2), (win_height / 2) - y);
    }
    if (screen_state == GAME_PLAY) {
        if (game_state)
            game_state->updateMouse(x - (win_width / 2), (win_height / 2) - y);
    }
}

void initStuff() {
    GLfloat light_a[] = {0.3f, 0.3f, 0.3f, 1.0f};  // ambient light
    GLfloat light_d[] = {1.0f, 1.0f, 1.0f, 1.0f};  // diffuse light
    GLfloat light_s[] = {1.0f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_a);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_d);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_s);
    GLfloat light_pos0[4] = {1.0f, -1.0f, 0.0f, 0.0f};
    GLfloat light_pos1[4] = {1.0f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);

    // readymenu light
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_d);
    glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glDisable(GL_COLOR_MATERIAL);  // If you enable this materials will stop
                                   // working
    glDisable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
}
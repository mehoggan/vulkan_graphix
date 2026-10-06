// Vulkan Earth
//<Insert Team Name Here>
//
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <new>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "math.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/GameState.h"
#include "vulkan_earth/GlobalSettings.h"
#include "vulkan_earth/LoadingScreen.h"
#include "vulkan_earth/MainMenu.h"
#include "vulkan_earth/PlayerFactory.h"
#include "vulkan_earth/PossibleGameStates.h"
#include "vulkan_earth/ReadyMenu.h"
#include "vulkan_earth/ShopMenu.h"
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/SubMenuLandscape.h"
#include "vulkan_earth/TerrainMaker.h"
#include "vulkan_graphix/OperatingSystem.h"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"
#include "vulkan_earth/MacroCrtdbg.h"

#ifdef new
#undef new
#endif

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;
namespace os = vulkan_graphix::os;

// GLUT's mouse button and state values, which the handlers below were
// written against.
constexpr std::int32_t c_glut_left_button = 0;
constexpr std::int32_t c_glut_down = 0;
constexpr std::int32_t c_glut_up = 1;

void resize(std::int32_t, std::int32_t);
bool draw();
void keyHandler(std::uint8_t, std::int32_t, std::int32_t);
void keyHandlerUp(std::uint8_t key, std::int32_t x, std::int32_t y);
void specKeyHandler(std::int32_t key, std::int32_t x, std::int32_t y);
void specKeyHandlerUp(std::int32_t key, std::int32_t x, std::int32_t y);
void mouseHandler(std::int32_t, std::int32_t, std::int32_t, std::int32_t);
void mouseMotionHandler(std::int32_t, std::int32_t);
extern void initSound();
void quitGame(bool delete_game_state);

std::int32_t screen_state;
std::int32_t prev_screen_state;
enum removeGlutStates { UP, DOWN };
std::int32_t win_width, win_height;
std::int32_t timer;
MainMenu* mainmenu;
ReadyMenu* readymenu;
ShopMenu* shopmenu;
GlobalSettings* global_settings;
PlayerFactory* player_factory;
GameState* game_state;
LoadingScreen* loading_screen;
os::Window* window;
bool quit_requested = false;

// GLUT_SCREEN_WIDTH/HEIGHT is the whole X11 screen - the combined
// virtual desktop spanning every monitor, not just the primary one
// (unlike Windows, where it's the primary display's resolution). Ask
// RandR for the primary monitor's real geometry instead, so the window
// created from it stays confined to that one monitor.
static void primaryMonitorGeometry(std::int32_t* pos_x,
  std::int32_t* pos_y,
  std::int32_t* width,
  std::int32_t* height) {
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
          primary ? XRRGetOutputInfo(display, resources, primary) : nullptr;
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

// glutTimerFunc(20, timerEvent, 1): the game advances one draw() per 20
// milliseconds at most - all of its animation and physics count frames.
constexpr std::chrono::milliseconds c_frame_interval(20);

// A debugging aid, standing in for a person at the keyboard and mouse:
// with VE_SCRIPT=<file>, each "<frame> <op> [args]" line is replayed at the
// start of that frame's draw() - "down x y" / "up x y" (left mouse button),
// "move x y", "key c" / "keyup c" (a character, or "space" / "esc"),
// "spec n" / "specup n" (a GLUT special key code), "capture <file.ppm>"
// (that frame, as a binary PPM), and "quit". VE_WINDOW=<w>x<h> overrides
// the window size, so captures line up across runs.
struct ScriptEvent {
    std::int64_t m_frame = 0;
    std::string m_op;
    std::string m_argument;
    std::int32_t m_x = 0;
    std::int32_t m_y = 0;
};
std::vector<ScriptEvent> script;
std::int64_t script_frame = 0;

void loadScript() {
    const char* path = std::getenv("VE_SCRIPT");
    if (path == nullptr) {
        return;
    }
    std::ifstream script_file(path);
    std::string line;
    while (std::getline(script_file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        std::istringstream fields(line);
        ScriptEvent event;
        fields >> event.m_frame >> event.m_op;
        if (event.m_op == "capture" || event.m_op == "key" ||
          event.m_op == "keyup" || event.m_op == "spec" ||
          event.m_op == "specup") {
            fields >> event.m_argument;
        } else {
            fields >> event.m_x >> event.m_y;
        }
        script.push_back(event);
    }
}

std::uint8_t scriptKey(const std::string& name) {
    if (name == "esc") return 27;
    if (name == "space") return ' ';
    return static_cast<std::uint8_t>(name[0]);
}

void runScriptFrame() {
    ++script_frame;
    for (const ScriptEvent& event : script) {
        if (event.m_frame != script_frame) continue;
        if (event.m_op == "down")
            mouseHandler(
              c_glut_left_button, c_glut_down, event.m_x, event.m_y);
        else if (event.m_op == "up")
            mouseHandler(c_glut_left_button, c_glut_up, event.m_x, event.m_y);
        else if (event.m_op == "move")
            mouseMotionHandler(event.m_x, event.m_y);
        else if (event.m_op == "key")
            keyHandler(scriptKey(event.m_argument), 0, 0);
        else if (event.m_op == "keyup")
            keyHandlerUp(scriptKey(event.m_argument), 0, 0);
        else if (event.m_op == "spec")
            specKeyHandler(std::atoi(event.m_argument.c_str()), 0, 0);
        else if (event.m_op == "specup")
            specKeyHandlerUp(std::atoi(event.m_argument.c_str()), 0, 0);
        else if (event.m_op == "capture")
            render::Renderer::instance().requestCapture(event.m_argument);
        else if (event.m_op == "quit")
            quitGame(true);
    }
}

// The game on top of the tutorials' Vulkan bring-up: TutorialBase owns the
// instance, device, and swapchain; the Renderer draws into them; X11 events
// are forwarded to the game's original GLUT-style handlers.
class VulkanEarthApp : public vulkan_graphix::TutorialBase {
public:
    ~VulkanEarthApp() override {
        vulkan_earth::releaseGameRendering();
        m_renderer.shutdown();
    }

    bool initializeRenderer() {
        return m_renderer.initialize(*this) &&
          vulkan_earth::initializeGameRendering(m_renderer);
    }

    bool draw() override {
        auto current_time = std::chrono::steady_clock::now();
        if (current_time < m_next_frame) {
            std::this_thread::sleep_until(m_next_frame);
            current_time = m_next_frame;
        }
        m_next_frame = current_time + c_frame_interval;
        if (!::draw()) {
            // The swapchain no longer matches the window.
            return onWindowSizeChanged();
        }
        return true;
    }

    bool onKey(const os::KeyEvent& event) override {
        // GLUT's keyboard callbacks get the character a character_key types
        // (Escape is 27, Enter 13, ...); its special callbacks get function
        // and arrow keys as GLUT_KEY_* codes.
        if (event.m_character != '\0') {
            const auto character_key =
              static_cast<std::uint8_t>(event.m_character);
            if (event.m_pressed) {
                keyHandler(character_key, 0, 0);
            } else {
                keyHandlerUp(character_key, 0, 0);
            }
            return true;
        }
        std::int32_t special = -1;
        if (event.m_keysym >= XK_F1 && event.m_keysym <= XK_F12) {
            special = static_cast<std::int32_t>(event.m_keysym - XK_F1) + 1;
        } else if (event.m_keysym == XK_Left) {
            special = 100;
        } else if (event.m_keysym == XK_Up) {
            special = 101;
        } else if (event.m_keysym == XK_Right) {
            special = 102;
        } else if (event.m_keysym == XK_Down) {
            special = 103;
        }
        if (special != -1) {
            if (event.m_pressed) {
                specKeyHandler(special, 0, 0);
            } else {
                specKeyHandlerUp(special, 0, 0);
            }
        }
        return true;
    }

    void onMouseButton(std::int32_t button,
      bool pressed,
      std::int32_t pos_x,
      std::int32_t pos_y) override {
        // X11 numbers buttons from 1; GLUT from 0 (left, middle, right).
        if (button >= 1 && button <= 3) {
            m_held_buttons += pressed ? 1 : -1;
        }
        mouseHandler(
          button - 1, pressed ? c_glut_down : c_glut_up, pos_x, pos_y);
    }

    void onMouseMove(std::int32_t pos_x, std::int32_t pos_y) override {
        // glutMotionFunc(): motion is only reported while a button is held.
        if (m_held_buttons > 0) {
            mouseMotionHandler(pos_x, pos_y);
        }
    }

    bool quitRequested() const override { return quit_requested; }

protected:
    void childClear() override {
        if (render::Renderer::hasInstance()) {
            m_renderer.releaseSwapchainResources();
        }
    }

    bool childOnWindowSizeChanged() override {
        if (!m_renderer.onSwapchainRecreated(*this)) {
            return false;
        }
        resize(vulkan_earth::windowWidth(), vulkan_earth::windowHeight());
        return true;
    }

private:
    render::Renderer m_renderer;
    std::chrono::steady_clock::time_point m_next_frame;
    std::int32_t m_held_buttons = 0;
};

int main() {
    screen_state = MAIN_MENU;
    prev_screen_state = screen_state;

    // The game opens its assets (textures, meshes, sounds) by paths
    // relative to its own directory, where the build copies them.
    std::error_code error;
    std::filesystem::current_path(
      vulkan_graphix::Tools::executableDir(), error);

    initSound();
    std::int32_t win_pos_x, win_pos_y;
    primaryMonitorGeometry(&win_pos_x, &win_pos_y, &win_width, &win_height);
    if (const char* size = std::getenv("VE_WINDOW")) {
        std::sscanf(size, "%dx%d", &win_width, &win_height);
    }
    loadScript();

    os::Window game_window;
    window = &game_window;
    if (!game_window.create(
          "VulkanEarth", win_pos_x, win_pos_y, win_width, win_height)) {
        return EXIT_FAILURE;
    }
    game_window.setKeyRepeat(false);

    VulkanEarthApp earth_app;
    if (!earth_app.prepareVulkan(game_window.getParameters()) ||
      !earth_app.initializeRenderer()) {
        return EXIT_FAILURE;
    }
    // The window can come up at a different size than requested.
    win_width = vulkan_earth::windowWidth();
    win_height = vulkan_earth::windowHeight();
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

    const bool loop_ok = game_window.renderingLoop(earth_app);
    if (!quit_requested) {
        quitGame(true);
    }
    delete loading_screen;
    loading_screen = nullptr;
    window = nullptr;
    return loop_ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

// What every one of the original's exit(0) calls did first: free the
// game's objects (except where it deliberately left game_state alone),
// then end the rendering loop.
void quitGame(bool delete_game_state) {
    delete mainmenu;
    delete readymenu;
    delete shopmenu;
    if (delete_game_state) {
        delete game_state;
    }
    delete global_settings;
    delete player_factory;
    mainmenu = nullptr;
    readymenu = nullptr;
    shopmenu = nullptr;
    game_state = nullptr;
    global_settings = nullptr;
    player_factory = nullptr;
    quit_requested = true;
}

void resize(std::int32_t width, std::int32_t height) {
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

// The menus' camera: looking down -z at the z = 0 plane from the distance
// where one unit is one pixel.
void menuLookAt(render::RenderContext& context) {
    std::int32_t distance = win_height / 2 * tan(1.04719755);
    context.setCamera(context.projection(),
      context.view() *
        glm::lookAt(math::Vec3<float>(0, 0, distance),
          math::Vec3<float>(0, 0, 0),
          math::Vec3<float>(0, 1, 0)));
}

// Draws one frame; returns false when the swapchain has to be rebuilt
// first.
bool draw() {
    runScriptFrame();
    if (quit_requested) {
        return true;
    }
    render::Renderer& renderer = render::Renderer::instance();
    // glClearColor(0, 0, 0, 0), glClearDepth(1.0f), then a fresh full-window
    // projection - near/far 1/1000000 for the menus, 100/100000000 in game
    // - and an identity modelview.
    render::RenderContext* context =
      renderer.beginFrame(math::Vec4<float>(0.0f));
    if (context == nullptr) {
        return false;
    }
    const float near_plane = screen_state != GAME_PLAY ? 1.0 : 100.0;
    const float far_plane =
      screen_state != GAME_PLAY ? 1000000.0 : 100000000.0;
    context->setViewport({0, 0, win_width, win_height});
    context->setCamera(
      vulkan_graphix::Tools::getPerspectiveProjectionMatrix(
        static_cast<float>(win_width) / static_cast<float>(win_height),
        60.0,
        near_plane,
        far_plane),
      math::Mat4<float>(1.0f));
    switch (screen_state) {
        case MAIN_MENU: {
            if (prev_screen_state != MAIN_MENU) {
                mainmenu->getSubMenuLandscape()->m_tm->prepareData(
                  0, 0, 0, 0, 0);
                window->setCursorVisible(true);
            }
            menuLookAt(*context);
            mainmenu->draw(*context);
            break;
        }
        case READY_MENU: {
            if (readymenu == nullptr) {
                menuLookAt(*context);
                loading_screen->draw(*context);
                // glutSwapBuffers(): the loading screen goes up while the
                // ready menu is built, and drawing carries on into the next
                // frame with the same camera.
                const math::Mat4<float> projection = context->projection();
                const math::Mat4<float> view = context->view();
                if (!renderer.endFrame()) {
                    return false;
                }
                readymenu = new ReadyMenu(win_width,
                  win_height,
                  0.01f,
                  global_settings,
                  player_factory,
                  &screen_state);
                context = renderer.beginFrame(math::Vec4<float>(0.0f));
                if (context == nullptr) {
                    return false;
                }
                context->setViewport({0, 0, win_width, win_height});
                context->setCamera(projection, view);
            }
            if (prev_screen_state != READY_MENU) {
                readymenu->updatePageInfo();
            }
            menuLookAt(*context);
            readymenu->draw(*context);
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
            menuLookAt(*context);
            if (screen_state == SHOP_MENU) shopmenu->draw(*context);
            break;
        }
        case GAME_PLAY: {
            if (game_state == nullptr) {
                game_state = new GameState(win_width,
                  win_height,
                  player_factory,
                  global_settings,
                  &screen_state);
                window->setCursorVisible(false);
            }
            if (readymenu) {
                delete readymenu;
                readymenu = nullptr;
            }
            if (shopmenu) {
                delete shopmenu;
                shopmenu = nullptr;
            }
            game_state->draw(*context);

            // World axes at the origin (red x, green y, blue z).
            const std::vector<render::UiVertex> axes = {
              {math::Vec3<float>(0, 0, 0),
                math::Vec4<float>(1, 0, 0, 1),
                math::Vec2<float>(0.0f)},
              {math::Vec3<float>(1000, 0, 0),
                math::Vec4<float>(1, 0, 0, 1),
                math::Vec2<float>(0.0f)},
              {math::Vec3<float>(0, 0, 0),
                math::Vec4<float>(0, 1, 0, 1),
                math::Vec2<float>(0.0f)},
              {math::Vec3<float>(0, 1000, 0),
                math::Vec4<float>(0, 1, 0, 1),
                math::Vec2<float>(0.0f)},
              {math::Vec3<float>(0, 0, 0),
                math::Vec4<float>(0, 0, 1, 1),
                math::Vec2<float>(0.0f)},
              {math::Vec3<float>(0, 0, 1000),
                math::Vec4<float>(0, 0, 1, 1),
                math::Vec2<float>(0.0f)}};
            context->drawTransient(
              axes, vulkan_earth::pipelines().m_ui_lines, nullptr);

            break;
        }
        case QUIT_GAME: {
            quitGame(true);
            break;
        }
        default: {
            cout << "Error" << endl;
        }
    }

    if (screen_state != prev_screen_state && !quit_requested) {
        if (readymenu)
            readymenu->updateNumPlayers(global_settings->getPlayerCount());
        if (shopmenu)
            shopmenu->updateNumPlayers(global_settings->getPlayerCount());
        prev_screen_state = screen_state;
    }

    return renderer.endFrame();
}

void keyHandler(std::uint8_t key, std::int32_t /*x*/, std::int32_t /*y*/) {
    if (screen_state == READY_MENU) {
        if (key == 27) {
            screen_state = QUIT_GAME;
        }
        if (readymenu) readymenu->keyTest(key);
    }
    if (screen_state == SHOP_MENU) {
        if (key == 27) {
            quitGame(true);  // this is just temporary, delete this later
            return;
        }
    }
    if (screen_state == MAIN_MENU) {
        switch (key) {
            case 27: {  // ESCAPE KEY
                quitGame(true);
                return;
            }
        }
    }
    if (screen_state == GAME_PLAY) {
        if (key == 27) {
            quitGame(false);  // game_state: UNCOMMENT ON RELEASE
            return;
        } else if (key == 'n')
            global_settings->getCurrentTerrain()->toggleWireframe();
        else if (key == 'm')
            screen_state =
              MAIN_MENU;  // this is just temporary, delete this later
        else
            game_state->handleKeyboardInput(key, true);
    }
}

void keyHandlerUp(std::uint8_t key, std::int32_t /*x*/, std::int32_t /*y*/) {
    if (screen_state == GAME_PLAY) {
        if (key == static_cast<std::int32_t>('l')) {
            global_settings->getCurrentTerrain()->toggleWireframe();
        } else {
            game_state->handleKeyboardInput(key, false);
        }
    }
}

void specKeyHandler(std::int32_t key, std::int32_t /*x*/, std::int32_t /*y*/) {
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

void specKeyHandlerUp(
  std::int32_t key, std::int32_t /*x*/, std::int32_t /*y*/) {
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

void mouseHandler(
  std::int32_t button, std::int32_t state, std::int32_t x, std::int32_t y) {
    switch (screen_state) {
        case MAIN_MENU: {
            if (button == c_glut_left_button) {
                if (state == c_glut_up) {
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
            if (button == c_glut_left_button) {
                if (state == c_glut_up) {
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
            if (button == c_glut_left_button) {
                if (state == c_glut_up) {
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

void mouseMotionHandler(std::int32_t x, std::int32_t y) {
    if (mainmenu->getActiveSubMenu() != nullptr) {
        if (mainmenu->getActiveSubMenu()->getUNIQUEIDENTIFIER() == 1) {
            mainmenu->getActiveSubMenu()->updateMouse(
              x - (win_width / 2), (win_height / 2) - y);
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

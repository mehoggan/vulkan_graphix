#ifndef VULKAN_GRAPHIX_OPERATINGSYSTEM_H
#define VULKAN_GRAPHIX_OPERATINGSYSTEM_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <dlfcn.h>
#include <cstdint>

#include <string>

namespace vulkan_graphix::os {

using LibraryHandle = void*;

// One keyboard press or release. keysym is the X11 KeySym (XK_Escape,
// XK_Left, XK_F1, ...); character is the ASCII character the key types
// under the current modifiers (e.g. 'a', 'A', ' ', 27 for Escape), or 0 for
// keys that type nothing (arrows, function keys, modifiers).
struct KeyEvent {
  std::uint64_t m_keysym;
  char m_character;
  bool m_pressed;
};

class ProjectBase {
public:
  ProjectBase();

  virtual ~ProjectBase();

  ProjectBase(const ProjectBase& other);

  ProjectBase& operator=(const ProjectBase& other);

  virtual bool readyToDraw() const;

  virtual bool onWindowSizeChanged() = 0;
  virtual bool draw() = 0;

  // Mouse input hooks. Default implementations do nothing, so tutorials
  // that don't need mouse input (i.e. all of them except Tutorial09) are
  // unaffected. `button` follows X11 convention: 1/2/3 = left/middle/
  // right, 4/5 = scroll wheel up/down (reported as a press with no
  // matching release).
  virtual void onMouseButton(std::int32_t button,
      bool pressed,
      std::int32_t pos_x,
      std::int32_t pos_y);
  virtual void onMouseMove(std::int32_t pos_x, std::int32_t pos_y);

  // Keyboard hook. Returns whether the rendering loop should keep
  // running. The default ends the loop on any key press - every
  // tutorial's long-standing "press a key to quit" behavior - and ignores
  // releases.
  virtual bool onKey(const KeyEvent& event);

  // Polled once per loop iteration: an application that decides to exit
  // on its own (e.g. a menu's Quit button) returns true to end the loop
  // cleanly. Default: never.
  virtual bool quitRequested() const;

protected:
  bool m_can_render;
};

class WindowParameters {
public:
  WindowParameters();

  Display* getDisplayPtr() const;
  Display*& getDisplayPtr();
  void setDisplayPtr(Display*& display_ptr);

  Window& getWindowHandle();

  void setWindowHandle(Window& handle);

private:
  Display* m_display_ptr;
  Window m_handle;
};

class Window {
public:
  Window();
  ~Window();

  WindowParameters getParameters() const;

  // 500x500 at (20, 20) - every tutorial's window.
  bool create(const std::string& title);
  bool create(const std::string& title,
      std::int32_t pos_x,
      std::int32_t pos_y,
      std::int32_t width,
      std::int32_t height);
  bool renderingLoop(ProjectBase& project);

  // With key repeat off, holding a key reports one press and one release
  // (X11's auto-repeat press/release pairs are suppressed), like GLUT's
  // glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF). On by default.
  void setKeyRepeat(bool enabled);
  void setCursorVisible(bool visible);

private:
  WindowParameters m_parameters;
  bool m_key_repeat = true;
  Cursor m_blank_cursor = None;
};

}  // namespace vulkan_graphix::os

#endif

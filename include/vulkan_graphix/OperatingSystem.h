#ifndef VULKAN_GRAPHIX_OPERATINGSYSTEM_H
#define VULKAN_GRAPHIX_OPERATINGSYSTEM_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <dlfcn.h>
#include <cstdint>

#include <cstdlib>
#include <cstring>
#include <string>

// TODO (mehoggan@gmail.com): This file needs class and function documentation.
// It is not clear what the purpose of this file is and how it should be used.
// It seems to be a wrapper around X11 and Vulkan for cheating a window and
// rendering to it, but it is not clear how it should be used in the context of
// the rest of the codebase.

namespace vulkan_graphix::os {

using LibraryHandle = void*;

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

    bool create(const std::string& title);
    bool renderingLoop(ProjectBase& project);

private:
    WindowParameters m_parameters;
};

}  // namespace vulkan_graphix::os

#endif

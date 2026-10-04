#include "vulkan_graphix/OperatingSystem.h"

#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <set>
#include <thread>

namespace vulkan_graphix::os {

ProjectBase::ProjectBase() :
        m_can_render(false) {}

ProjectBase::~ProjectBase() = default;

ProjectBase::ProjectBase(const ProjectBase& other) = default;

ProjectBase& ProjectBase::operator=(const ProjectBase& other) = default;

bool ProjectBase::readyToDraw() const { return m_can_render; }

void ProjectBase::onMouseButton(std::int32_t /*button*/,
                                bool /*pressed*/,
                                std::int32_t /*pos_x*/,
                                std::int32_t /*pos_y*/) {}

void ProjectBase::onMouseMove(std::int32_t /*pos_x*/, std::int32_t /*pos_y*/) {
}

bool ProjectBase::onKey(const KeyEvent& event) { return !event.m_pressed; }

bool ProjectBase::quitRequested() const { return false; }

WindowParameters::WindowParameters() :
        m_display_ptr(nullptr),
        m_handle{} {}

Display* WindowParameters::getDisplayPtr() const { return m_display_ptr; }

Display*& WindowParameters::getDisplayPtr() { return m_display_ptr; }

void WindowParameters::setDisplayPtr(Display*& display_ptr) {
    m_display_ptr = display_ptr;
}

::Window& WindowParameters::getWindowHandle() { return m_handle; }

void WindowParameters::setWindowHandle(::Window& handle) { m_handle = handle; }

Window::Window() = default;

Window::~Window() {
    // getDisplayPtr() is null if create() was never called or failed (e.g.
    // XOpenDisplay() itself failing) - XDestroyWindow/XCloseDisplay on a
    // null Display* is undefined behavior, so skip both in that case.
    if (m_parameters.getDisplayPtr() != nullptr) {
        if (m_blank_cursor != None) {
            XFreeCursor(m_parameters.getDisplayPtr(), m_blank_cursor);
        }
        XDestroyWindow(m_parameters.getDisplayPtr(),
                       m_parameters.getWindowHandle());
        XCloseDisplay(m_parameters.getDisplayPtr());
    }
}

WindowParameters Window::getParameters() const { return m_parameters; }

bool Window::create(const std::string& title) {
    return create(title, 20, 20, 500, 500);
}

bool Window::create(const std::string& title,
                    std::int32_t pos_x,
                    std::int32_t pos_y,
                    std::int32_t width,
                    std::int32_t height) {
    Display* display_ptr = XOpenDisplay(nullptr);
    m_parameters.setDisplayPtr(display_ptr);
    if (m_parameters.getDisplayPtr() == nullptr) {
        return false;
    }

    std::int32_t default_screen = DefaultScreen(m_parameters.getDisplayPtr());

    ::Window handle = XCreateSimpleWindow(
            m_parameters.getDisplayPtr(),
            DefaultRootWindow(m_parameters.getDisplayPtr()),
            pos_x,
            pos_y,
            static_cast<std::uint32_t>(width),
            static_cast<std::uint32_t>(height),
            1,
            BlackPixel(m_parameters.getDisplayPtr(), default_screen),
            WhitePixel(m_parameters.getDisplayPtr(), default_screen));
    m_parameters.setWindowHandle(handle);

    // Ask the window manager to honor the requested position, not just the
    // size (it otherwise usually picks its own placement).
    XSizeHints size_hints{};
    size_hints.flags = USPosition | USSize;
    size_hints.x = pos_x;
    size_hints.y = pos_y;
    size_hints.width = width;
    size_hints.height = height;
    XSetStandardProperties(m_parameters.getDisplayPtr(),
                           m_parameters.getWindowHandle(),
                           title.c_str(),
                           title.c_str(),
                           None,
                           nullptr,
                           0,
                           &size_hints);
    XSelectInput(m_parameters.getDisplayPtr(),
                 m_parameters.getWindowHandle(),
                 ExposureMask | KeyPressMask | KeyReleaseMask |
                         StructureNotifyMask | ButtonPressMask |
                         ButtonReleaseMask | PointerMotionMask);

    return true;
}

void Window::setKeyRepeat(bool enabled) {
    m_key_repeat = enabled;
    if (!enabled && m_parameters.getDisplayPtr() != nullptr) {
        // Auto-repeat then reports repeated KeyPresses with no fake
        // KeyRelease in between; renderingLoop() drops those repeats.
        XkbSetDetectableAutoRepeat(
                m_parameters.getDisplayPtr(), True, nullptr);
    }
}

void Window::setCursorVisible(bool visible) {
    Display* display_ptr = m_parameters.getDisplayPtr();
    if (display_ptr == nullptr) {
        return;
    }
    if (visible) {
        XUndefineCursor(display_ptr, m_parameters.getWindowHandle());
    } else {
        if (m_blank_cursor == None) {
            const std::array<char, 1> empty_bits = {0};
            const Pixmap blank =
                    XCreateBitmapFromData(display_ptr,
                                          m_parameters.getWindowHandle(),
                                          empty_bits.data(),
                                          1,
                                          1);
            XColor black{};
            m_blank_cursor = XCreatePixmapCursor(
                    display_ptr, blank, blank, &black, &black, 0, 0);
            XFreePixmap(display_ptr, blank);
        }
        XDefineCursor(
                display_ptr, m_parameters.getWindowHandle(), m_blank_cursor);
    }
    XFlush(display_ptr);
}

bool Window::renderingLoop(ProjectBase& project) {
    // Prepare notification for window destruction
    Atom delete_window_atom;
    delete_window_atom = XInternAtom(
            m_parameters.getDisplayPtr(), "WM_DELETE_WINDOW", false);
    Display* display_ptr = m_parameters.getDisplayPtr();
    ::Window& handle = m_parameters.getWindowHandle();

    XSetWMProtocols(display_ptr, handle, &delete_window_atom, 1);

    // Display window
    XClearWindow(display_ptr, handle);
    XMapWindow(display_ptr, handle);

    // Main message loop
    XEvent event;
    std::set<std::uint32_t> held_keys;
    bool loop = true;
    bool resize = false;
    bool result = true;

    while (loop) {
        if (project.quitRequested()) {
            break;
        }
        if (XPending(m_parameters.getDisplayPtr())) {
            XNextEvent(m_parameters.getDisplayPtr(), &event);
            switch (event.type) {
                    // Process events
                case ConfigureNotify: {
                    static std::int32_t width = event.xconfigure.width;
                    static std::int32_t height = event.xconfigure.height;

                    if (((event.xconfigure.width > 0) &&
                         (event.xconfigure.width != width)) ||
                        ((event.xconfigure.height > 0) &&
                         (event.xconfigure.height != height))) {
                        width = event.xconfigure.width;
                        height = event.xconfigure.height;
                        resize = true;
                    }
                } break;
                case KeyPress:
                case KeyRelease: {
                    const bool pressed = event.type == KeyPress;
                    const std::uint32_t keycode = event.xkey.keycode;
                    if (!m_key_repeat) {
                        if (pressed && held_keys.count(keycode) != 0) {
                            break;  // an auto-repeat of a held key
                        }
                        if (pressed) {
                            held_keys.insert(keycode);
                        } else {
                            held_keys.erase(keycode);
                        }
                    }
                    std::array<char, 8> text = {};
                    KeySym keysym = NoSymbol;
                    const std::int32_t length = XLookupString(
                            &event.xkey,
                            text.data(),
                            static_cast<std::int32_t>(text.size()),
                            &keysym,
                            nullptr);
                    const KeyEvent key_event{
                            static_cast<std::uint64_t>(keysym),
                            length == 1 ? text[0] : '\0',
                            pressed};
                    if (!project.onKey(key_event)) {
                        loop = false;
                    }
                } break;
                case DestroyNotify:
                    loop = false;
                    break;
                case ClientMessage:
                    if (static_cast<std::uint32_t>(event.xclient.data.l[0]) ==
                        delete_window_atom) {
                        loop = false;
                    }
                    break;
                case ButtonPress:
                    project.onMouseButton(
                            static_cast<std::int32_t>(event.xbutton.button),
                            true,
                            event.xbutton.x,
                            event.xbutton.y);
                    break;
                case ButtonRelease:
                    project.onMouseButton(
                            static_cast<std::int32_t>(event.xbutton.button),
                            false,
                            event.xbutton.x,
                            event.xbutton.y);
                    break;
                case MotionNotify:
                    project.onMouseMove(event.xmotion.x, event.xmotion.y);
                    break;
            }
        } else {
            // Draw
            if (resize) {
                resize = false;
                if (!project.onWindowSizeChanged()) {
                    result = false;
                    break;
                }
            }
            if (project.readyToDraw()) {
                if (!project.draw()) {
                    result = false;
                    break;
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

    return result;
}
}  // namespace vulkan_graphix::os

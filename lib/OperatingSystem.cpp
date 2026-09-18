#include "vulkan_graphix/OperatingSystem.h"

#include <thread>

namespace vulkan_graphix::os {

ProjectBase::ProjectBase() : m_can_render(false) {}

ProjectBase::~ProjectBase() = default;

ProjectBase::ProjectBase(const ProjectBase& other) = default;

ProjectBase& ProjectBase::operator=(const ProjectBase& other) = default;

bool ProjectBase::readyToDraw() const { return m_can_render; }

void ProjectBase::onMouseButton(int /*button*/,
                                bool /*pressed*/,
                                int /*pos_x*/,
                                int /*pos_y*/) {}

void ProjectBase::onMouseMove(int /*pos_x*/, int /*pos_y*/) {}

WindowParameters::WindowParameters() : m_display_ptr(nullptr), m_handle{} {}

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
        XDestroyWindow(m_parameters.getDisplayPtr(),
                       m_parameters.getWindowHandle());
        XCloseDisplay(m_parameters.getDisplayPtr());
    }
}

WindowParameters Window::getParameters() const { return m_parameters; }

bool Window::create(const std::string& title) {
    Display* display_ptr = XOpenDisplay(nullptr);
    m_parameters.setDisplayPtr(display_ptr);
    if (m_parameters.getDisplayPtr() == nullptr) {
        return false;
    }

    int default_screen = DefaultScreen(m_parameters.getDisplayPtr());

    ::Window handle = XCreateSimpleWindow(
            m_parameters.getDisplayPtr(),
            DefaultRootWindow(m_parameters.getDisplayPtr()),
            20,
            20,
            500,
            500,
            1,
            BlackPixel(m_parameters.getDisplayPtr(), default_screen),
            WhitePixel(m_parameters.getDisplayPtr(), default_screen));
    m_parameters.setWindowHandle(handle);

    // XSync( m_parameters.m_display_ptr, false );
    XSetStandardProperties(m_parameters.getDisplayPtr(),
                           m_parameters.getWindowHandle(),
                           title.c_str(),
                           title.c_str(),
                           None,
                           nullptr,
                           0,
                           nullptr);
    XSelectInput(m_parameters.getDisplayPtr(),
                 m_parameters.getWindowHandle(),
                 ExposureMask | KeyPressMask | StructureNotifyMask |
                         ButtonPressMask | ButtonReleaseMask |
                         PointerMotionMask);

    return true;
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
    bool loop = true;
    bool resize = false;
    bool result = true;

    while (loop) {
        if (XPending(m_parameters.getDisplayPtr())) {
            XNextEvent(m_parameters.getDisplayPtr(), &event);
            switch (event.type) {
                    // Process events
                case ConfigureNotify: {
                    static int width = event.xconfigure.width;
                    static int height = event.xconfigure.height;

                    if (((event.xconfigure.width > 0) &&
                         (event.xconfigure.width != width)) ||
                        ((event.xconfigure.height > 0) &&
                         (event.xconfigure.width != height))) {
                        width = event.xconfigure.width;
                        height = event.xconfigure.height;
                        resize = true;
                    }
                } break;
                case KeyPress:
                    loop = false;
                    break;
                case DestroyNotify:
                    loop = false;
                    break;
                case ClientMessage:
                    if (static_cast<unsigned int>(event.xclient.data.l[0]) ==
                        delete_window_atom) {
                        loop = false;
                    }
                    break;
                case ButtonPress:
                    project.onMouseButton(
                            static_cast<int>(event.xbutton.button),
                            true,
                            event.xbutton.x,
                            event.xbutton.y);
                    break;
                case ButtonRelease:
                    project.onMouseButton(
                            static_cast<int>(event.xbutton.button),
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

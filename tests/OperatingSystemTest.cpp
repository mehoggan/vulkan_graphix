// Drives os::Window::renderingLoop() - the real X11 event loop every
// tutorialNN_runner uses - with a minimal fake ProjectBase and synthetic
// X11 events sent from a second Display connection (XSendEvent), rather
// than the individual create*()/draw() calls the TutorialNNIntegrationTest
// binaries exercise directly. Needs a live X11 DISPLAY, same as those.

#include <chrono>
#include <thread>

#include <gtest/gtest.h>

#include "vulkan_graphix/OperatingSystem.h"

#include "IntegrationTestCommon.h"

namespace {

class FakeProject : public vulkan_graphix::os::ProjectBase {
public:
    bool onWindowSizeChanged() override {
        ++resize_count;
        return true;
    }

    bool draw() override {
        ++draw_count;
        return true;
    }

    bool readyToDraw() const override { return true; }

    void onMouseButton(int button,
                       bool pressed,
                       int pos_x,
                       int pos_y) override {
        if (pressed) {
            ++button_press_count;
        } else {
            ++button_release_count;
        }
        last_button = button;
        last_x = pos_x;
        last_y = pos_y;
    }

    void onMouseMove(int pos_x, int pos_y) override {
        ++move_count;
        last_move_x = pos_x;
        last_move_y = pos_y;
    }

    int resize_count = 0;
    int draw_count = 0;
    int button_press_count = 0;
    int button_release_count = 0;
    int move_count = 0;
    int last_button = 0;
    int last_x = 0;
    int last_y = 0;
    int last_move_x = 0;
    int last_move_y = 0;
};

void sendButtonEvent(Display* send_display,
                     ::Window handle,
                     int event_type,
                     unsigned long mask,
                     unsigned int button,
                     int pos_x,
                     int pos_y) {
    XEvent event{};
    event.xbutton.type = event_type;
    event.xbutton.display = send_display;
    event.xbutton.window = handle;
    event.xbutton.root = DefaultRootWindow(send_display);
    event.xbutton.subwindow = None;
    event.xbutton.time = CurrentTime;
    event.xbutton.x = pos_x;
    event.xbutton.y = pos_y;
    event.xbutton.x_root = pos_x;
    event.xbutton.y_root = pos_y;
    event.xbutton.state = 0;
    event.xbutton.button = button;
    event.xbutton.same_screen = True;
    XSendEvent(send_display, handle, False, mask, &event);
}

void sendMotionEvent(Display* send_display,
                     ::Window handle,
                     int pos_x,
                     int pos_y) {
    XEvent event{};
    event.xmotion.type = MotionNotify;
    event.xmotion.display = send_display;
    event.xmotion.window = handle;
    event.xmotion.root = DefaultRootWindow(send_display);
    event.xmotion.subwindow = None;
    event.xmotion.time = CurrentTime;
    event.xmotion.x = pos_x;
    event.xmotion.y = pos_y;
    event.xmotion.x_root = pos_x;
    event.xmotion.y_root = pos_y;
    event.xmotion.is_hint = NotifyNormal;
    event.xmotion.same_screen = True;
    XSendEvent(send_display, handle, False, PointerMotionMask, &event);
}

void sendDeleteWindowMessage(Display* send_display,
                             ::Window handle,
                             Atom wm_protocols,
                             Atom wm_delete_window) {
    XEvent event{};
    event.xclient.type = ClientMessage;
    event.xclient.display = send_display;
    event.xclient.window = handle;
    event.xclient.message_type = wm_protocols;
    event.xclient.format = 32;
    event.xclient.data.l[0] = static_cast<long>(wm_delete_window);
    XSendEvent(send_display, handle, False, NoEventMask, &event);
}

}  // namespace

TEST(OperatingSystemTest, RenderingLoopProcessesEventsAndExitsOnClose) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live X11 server)";
    }

    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("os-test"));
    ::Window handle = window.getParameters().getWindowHandle();

    Display* send_display = XOpenDisplay(nullptr);
    ASSERT_NE(send_display, nullptr);
    Atom wm_protocols = XInternAtom(send_display, "WM_PROTOCOLS", False);
    Atom wm_delete_window =
            XInternAtom(send_display, "WM_DELETE_WINDOW", False);

    FakeProject project;
    bool loop_result = false;
    std::thread loop_thread(
            [&]() { loop_result = window.renderingLoop(project); });

    // Give XMapWindow/XSelectInput time to take effect before driving events
    // at the window - two distinct sizes guarantee a real resize is
    // detected regardless of whether mapping itself also emits an initial
    // ConfigureNotify (see OperatingSystem.cpp's renderingLoop comment
    // about the static width/height baseline). These margins are
    // deliberately generous (observed flaking at 200/100/100ms under the
    // CPU contention of a concurrent full `make -j8` rebuild, which can
    // starve loop_thread of its first time slice) rather than tight,
    // since this test's own runtime cost is trivial either way.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    XResizeWindow(send_display, handle, 640, 640);
    XFlush(send_display);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    XResizeWindow(send_display, handle, 800, 800);
    XFlush(send_display);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    sendButtonEvent(send_display,
                    handle,
                    ButtonPress,
                    ButtonPressMask,
                    Button1,
                    100,
                    100);
    sendMotionEvent(send_display, handle, 120, 110);
    sendButtonEvent(send_display,
                    handle,
                    ButtonRelease,
                    ButtonReleaseMask,
                    Button1,
                    120,
                    110);
    XFlush(send_display);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    sendDeleteWindowMessage(
            send_display, handle, wm_protocols, wm_delete_window);
    XFlush(send_display);

    loop_thread.join();
    XCloseDisplay(send_display);

    EXPECT_TRUE(loop_result);
    EXPECT_GT(project.resize_count, 0);
    EXPECT_GT(project.draw_count, 0);
    EXPECT_EQ(project.button_press_count, 1);
    EXPECT_EQ(project.button_release_count, 1);
    EXPECT_GT(project.move_count, 0);
    EXPECT_EQ(project.last_button, static_cast<int>(Button1));
}

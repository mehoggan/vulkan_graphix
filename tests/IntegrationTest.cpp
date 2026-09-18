// Integration tests: unlike MathTest.cpp/VertexTypesTest.cpp (header-only,
// pure logic), these link against libvulkan_graphix.la and drive each
// tutorial through a real Vulkan device and X11 window - the only way to
// get any coverage on lib/*.cpp, which is otherwise 0% (see docs/
// vulkan_pipeline_breakdown.md's coverage notes). Each test creates a real
// window, runs a tutorial's full setup sequence (matching its
// binNN_main.cpp), draws a few frames, and tears down.
//
// These need a live Vulkan driver and X11 display, so every test skips
// itself via GTEST_SKIP() when DISPLAY isn't set rather than failing -
// that keeps `make check` safe on headless machines without Xvfb/a
// software rasterizer, while still exercising the real code path here.

#include <cstdlib>

#include <gtest/gtest.h>

#include "vulkan_graphix/OperatingSystem.h"
#include "vulkan_graphix/Tutorial01.h"
#include "vulkan_graphix/Tutorial02.h"

namespace {

bool hasDisplay() { return std::getenv("DISPLAY") != nullptr; }

constexpr int c_draw_iterations = 3;

}  // namespace

TEST(Tutorial01IntegrationTest, PrepareVulkanSucceeds) {
    if (!hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver, "
                        "no window required for Tutorial01)";
    }

    vulkan_graphix::Tutorial01 tutorial(false);
    EXPECT_TRUE(tutorial.prepareVulkan());
}

TEST(Tutorial02IntegrationTest, FullLifecycle) {
    if (!hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                        "and X11 window)";
    }

    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("integration-02"));

    vulkan_graphix::Tutorial02 tutorial;
    ASSERT_TRUE(tutorial.prepareVulkan(window.getParameters()));
    ASSERT_TRUE(tutorial.createSwapChain());
    ASSERT_TRUE(tutorial.createCommandBuffers());

    for (int i = 0; i < c_draw_iterations; ++i) {
        EXPECT_TRUE(tutorial.draw());
    }

    EXPECT_TRUE(tutorial.onWindowSizeChanged());

    for (int i = 0; i < c_draw_iterations; ++i) {
        EXPECT_TRUE(tutorial.draw());
    }
}

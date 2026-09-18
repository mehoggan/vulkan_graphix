// Integration tests: unlike MathTest.cpp/VertexTypesTest.cpp (header-only,
// pure logic), these link against libvulkan_graphix.la and drive a
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

#include <gtest/gtest.h>

#include "vulkan_graphix/Tutorial01.h"

#include "IntegrationTestCommon.h"

TEST(Tutorial01IntegrationTest, PrepareVulkanSucceeds) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver, "
                        "no window required for Tutorial01)";
    }

    vulkan_graphix::Tutorial01 tutorial(false);
    EXPECT_TRUE(tutorial.prepareVulkan());
}

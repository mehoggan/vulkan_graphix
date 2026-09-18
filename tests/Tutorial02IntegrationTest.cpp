// See Tutorial01IntegrationTest.cpp for why these are split one tutorial
// per translation unit, and IntegrationTestCommon.h for the shared
// helpers.

#include <gtest/gtest.h>

#include "vulkan_graphix/OperatingSystem.h"
#include "vulkan_graphix/Tutorial02.h"

#include "IntegrationTestCommon.h"

TEST(Tutorial02IntegrationTest, FullLifecycle) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                        "and X11 window)";
    }

    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("integration-02"));

    vulkan_graphix::Tutorial02 tutorial;
    ASSERT_TRUE(tutorial.prepareVulkan(window.getParameters()));
    ASSERT_TRUE(tutorial.createSwapChain());
    ASSERT_TRUE(tutorial.createCommandBuffers());

    for (int i = 0; i < vulkan_graphix::test::c_draw_iterations; ++i) {
        EXPECT_TRUE(tutorial.draw());
    }

    EXPECT_TRUE(tutorial.onWindowSizeChanged());

    for (int i = 0; i < vulkan_graphix::test::c_draw_iterations; ++i) {
        EXPECT_TRUE(tutorial.draw());
    }
}

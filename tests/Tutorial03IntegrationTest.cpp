// See Tutorial01IntegrationTest.cpp for why these are split one tutorial
// per translation unit, and IntegrationTestCommon.h for the shared
// helpers.

#include <memory>

#include <gtest/gtest.h>

#include "vulkan_graphix/OperatingSystem.h"
#include "vulkan_graphix/Tutorial03.h"

#include "IntegrationTestCommon.h"

TEST(Tutorial03IntegrationTest, FullLifecycle) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                        "and X11 window)";
    }

    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("integration-03"));

    auto tutorial = std::make_shared<vulkan_graphix::Tutorial03>();
    ASSERT_TRUE(tutorial->prepareVulkan(window.getParameters()));
    ASSERT_TRUE(tutorial->createRenderPass());
    ASSERT_TRUE(tutorial->createFramebuffers());
    ASSERT_TRUE(tutorial->createPipeline());
    ASSERT_TRUE(tutorial->createSemaphores());
    ASSERT_TRUE(tutorial->createCommandBuffers());
    ASSERT_TRUE(tutorial->recordCommandBuffers());

    for (int i = 0; i < vulkan_graphix::test::c_draw_iterations; ++i) {
        EXPECT_TRUE(tutorial->draw());
    }

    EXPECT_TRUE(tutorial->onWindowSizeChanged());

    for (int i = 0; i < vulkan_graphix::test::c_draw_iterations; ++i) {
        EXPECT_TRUE(tutorial->draw());
    }
}

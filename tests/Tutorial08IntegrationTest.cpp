// See Tutorial01IntegrationTest.cpp for why these are split one tutorial
// per translation unit, and IntegrationTestCommon.h for the shared
// helpers.

#include <memory>

#include <gtest/gtest.h>

#include "vulkan_graphix/OperatingSystem.h"
#include "vulkan_graphix/Tutorial08.h"

#include "IntegrationTestCommon.h"

TEST(Tutorial08IntegrationTest, FullLifecycle) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                        "and X11 window)";
    }

    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("integration-08"));

    auto tutorial = std::make_shared<vulkan_graphix::Tutorial08>();
    ASSERT_TRUE(tutorial->prepareVulkan(window.getParameters()));
    ASSERT_TRUE(tutorial->createRenderingResources());
    ASSERT_TRUE(tutorial->createStagingBuffer());
    ASSERT_TRUE(tutorial->createDepthResources());
    ASSERT_TRUE(tutorial->createUniformBuffer());
    ASSERT_TRUE(tutorial->createDescriptorSetLayout());
    ASSERT_TRUE(tutorial->createDescriptorPool());
    ASSERT_TRUE(tutorial->allocateDescriptorSet());
    ASSERT_TRUE(tutorial->updateDescriptorSet());
    ASSERT_TRUE(tutorial->createRenderPass());
    ASSERT_TRUE(tutorial->createPipelineLayout());
    ASSERT_TRUE(tutorial->createPipeline());
    ASSERT_TRUE(tutorial->createVertexBuffer());
    ASSERT_TRUE(tutorial->createIndexBuffer());

    for (int i = 0; i < vulkan_graphix::test::c_draw_iterations; ++i) {
        EXPECT_TRUE(tutorial->draw());
    }

    EXPECT_TRUE(tutorial->onWindowSizeChanged());

    for (int i = 0; i < vulkan_graphix::test::c_draw_iterations; ++i) {
        EXPECT_TRUE(tutorial->draw());
    }
}

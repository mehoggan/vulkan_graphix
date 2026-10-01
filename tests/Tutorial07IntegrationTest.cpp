// See Tutorial01IntegrationTest.cpp for why these are split one tutorial
// per translation unit, and IntegrationTestCommon.h for the shared
// helpers.

#include <cstdint>
#include <memory>

#include <gtest/gtest.h>

#include "Tutorial07.h"
#include "vulkan_graphix/OperatingSystem.h"

#include "IntegrationTestCommon.h"

TEST(Tutorial07IntegrationTest, FullLifecycle) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                        "and X11 window)";
    }

    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("integration-07"));

    auto tutorial = std::make_shared<vulkan_graphix::Tutorial07>();
    ASSERT_TRUE(tutorial->prepareVulkan(window.getParameters()));
    ASSERT_TRUE(tutorial->createRenderingResources());
    ASSERT_TRUE(tutorial->createStagingBuffer());
    ASSERT_TRUE(tutorial->createTexture());
    ASSERT_TRUE(tutorial->createUniformBuffer());
    ASSERT_TRUE(tutorial->createDescriptorSetLayout());
    ASSERT_TRUE(tutorial->createDescriptorPool());
    ASSERT_TRUE(tutorial->allocateDescriptorSet());
    ASSERT_TRUE(tutorial->updateDescriptorSet());
    ASSERT_TRUE(tutorial->createRenderPass());
    ASSERT_TRUE(tutorial->createPipelineLayout());
    ASSERT_TRUE(tutorial->createPipeline());
    ASSERT_TRUE(tutorial->createVertexBuffer());

    for (std::int32_t i = 0; i < vulkan_graphix::test::c_draw_iterations;
         ++i) {
        EXPECT_TRUE(tutorial->draw());
    }

    EXPECT_TRUE(tutorial->onWindowSizeChanged());

    for (std::int32_t i = 0; i < vulkan_graphix::test::c_draw_iterations;
         ++i) {
        EXPECT_TRUE(tutorial->draw());
    }
}

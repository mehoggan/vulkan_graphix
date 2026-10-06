// See Tutorial01IntegrationTest.cpp for why these are split one tutorial
// per translation unit, and IntegrationTestCommon.h for the shared
// helpers.

#include <cstdint>
#include <memory>

#include <gtest/gtest.h>

#include "Tutorial04.h"
#include "vulkan_graphix/OperatingSystem.h"

#include "IntegrationTestCommon.h"

TEST(Tutorial04IntegrationTest, FullLifecycle) {
  if (!vulkan_graphix::test::hasDisplay()) {
    GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                    "and X11 window)";
  }

  vulkan_graphix::os::Window window;
  ASSERT_TRUE(window.create("integration-04"));

  auto tutorial = std::make_shared<vulkan_graphix::Tutorial04>();
  ASSERT_TRUE(tutorial->prepareVulkan(window.getParameters()));
  ASSERT_TRUE(tutorial->createRenderPass());
  ASSERT_TRUE(tutorial->createPipeline());
  ASSERT_TRUE(tutorial->createVertexBuffer());
  ASSERT_TRUE(tutorial->createRenderingResources());

  for (std::int32_t i = 0; i < vulkan_graphix::test::c_draw_iterations; ++i) {
    EXPECT_TRUE(tutorial->draw());
  }

  EXPECT_TRUE(tutorial->onWindowSizeChanged());

  for (std::int32_t i = 0; i < vulkan_graphix::test::c_draw_iterations; ++i) {
    EXPECT_TRUE(tutorial->draw());
  }
}

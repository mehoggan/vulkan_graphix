// Split out from TutorialBaseFaultInjectionTest.cpp into its own process:
// this is the one test in the fault-injection group that ends up with a
// real, live VkDevice (and a real swapchain) by the time its
// TestableTutorialBase destructs - see TutorialBaseFaultInjectionCommon.h's
// header comment for why that means it can't share a process with another
// such test.

#include <gtest/gtest.h>

#include "vulkan_graphix/VulkanFunctions.h"

#include "IntegrationTestCommon.h"
#include "TutorialBaseFaultInjectionCommon.h"

using vulkan_graphix::test::bringUpThroughDeviceQueue;
using vulkan_graphix::test::TestableTutorialBase;

TEST(TutorialBaseFaultInjectionSwapChainTest,
     CreateSwapChainImageViewsFailsWhenCreateImageViewFails) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                        "and X11 window)";
    }
    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("fault-injection-image-views"));

    TestableTutorialBase tutorial;
    ASSERT_TRUE(bringUpThroughDeviceQueue(tutorial, window.getParameters()));
    // Runs createSwapChainImageViews() as its last step, populating
    // m_vulkan_common_parameters's image list for real first.
    ASSERT_TRUE(tutorial.createSwapChain());

    auto real_fn = vulkan_graphix::vkCreateImageView;
    vulkan_graphix::vkCreateImageView = [](VkDevice,
                                           const VkImageViewCreateInfo*,
                                           const VkAllocationCallbacks*,
                                           VkImageView*) -> VkResult {
        return VK_ERROR_INITIALIZATION_FAILED;
    };

    EXPECT_FALSE(tutorial.createSwapChainImageViews());

    vulkan_graphix::vkCreateImageView = real_fn;
}

// Fault-injects Tutorial04's own create*() failure branches, one per
// public method, by swapping the mutable vulkan_graphix::vkSomething
// function-pointer global its first underlying Vulkan call goes through
// for a fake that returns a chosen failure VkResult - see
// TutorialBaseFaultInjectionTest.cpp's header comment for the full
// rationale and TutorialBaseFaultInjectionCommon.h's for why this all has
// to share ONE real device across ONE TEST() rather than one real device
// per fault case: brings the tutorial up through a real prepareVulkan()
// exactly once, then reuses that same live device for every fault
// assertion below (each one calls its target create*() a second, faulted
// time - a harmless re-attempt for test purposes, since a failed call
// returns early without touching state further).

#include <gtest/gtest.h>

#include "vulkan_graphix/OperatingSystem.h"
#include "vulkan_graphix/Tutorial04.h"
#include "vulkan_graphix/VulkanFunctions.h"

#include "IntegrationTestCommon.h"

TEST(Tutorial04FaultInjectionTest,
     EachCreateMethodFailsWhenItsFirstVulkanCallFails) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                        "and X11 window)";
    }
    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("fault-injection-tutorial04"));

    vulkan_graphix::Tutorial04 tutorial;
    ASSERT_TRUE(tutorial.prepareVulkan(window.getParameters()));

    // createRenderPass() fails when vkCreateRenderPass fails.
    {
        auto real_fn = vulkan_graphix::vkCreateRenderPass;
        vulkan_graphix::vkCreateRenderPass = [](VkDevice,
                                                const VkRenderPassCreateInfo*,
                                                const VkAllocationCallbacks*,
                                                VkRenderPass*) -> VkResult {
            return VK_ERROR_INITIALIZATION_FAILED;
        };
        EXPECT_FALSE(tutorial.createRenderPass());
        vulkan_graphix::vkCreateRenderPass = real_fn;
    }
    ASSERT_TRUE(tutorial.createRenderPass());

    // createPipeline() fails when its first shader module fails to build.
    {
        auto real_fn = vulkan_graphix::vkCreateShaderModule;
        vulkan_graphix::vkCreateShaderModule =
                [](VkDevice,
                   const VkShaderModuleCreateInfo*,
                   const VkAllocationCallbacks*,
                   VkShaderModule*) -> VkResult {
            return VK_ERROR_INITIALIZATION_FAILED;
        };
        EXPECT_FALSE(tutorial.createPipeline());
        vulkan_graphix::vkCreateShaderModule = real_fn;
    }

    // createVertexBuffer() fails when vkCreateBuffer fails.
    {
        auto real_fn = vulkan_graphix::vkCreateBuffer;
        vulkan_graphix::vkCreateBuffer = [](VkDevice,
                                            const VkBufferCreateInfo*,
                                            const VkAllocationCallbacks*,
                                            VkBuffer*) -> VkResult {
            return VK_ERROR_INITIALIZATION_FAILED;
        };
        EXPECT_FALSE(tutorial.createVertexBuffer());
        vulkan_graphix::vkCreateBuffer = real_fn;
    }

    // createRenderingResources() fails when its command pool fails.
    {
        auto real_fn = vulkan_graphix::vkCreateCommandPool;
        vulkan_graphix::vkCreateCommandPool =
                [](VkDevice,
                   const VkCommandPoolCreateInfo*,
                   const VkAllocationCallbacks*,
                   VkCommandPool*) -> VkResult {
            return VK_ERROR_INITIALIZATION_FAILED;
        };
        EXPECT_FALSE(tutorial.createRenderingResources());
        vulkan_graphix::vkCreateCommandPool = real_fn;
    }
}

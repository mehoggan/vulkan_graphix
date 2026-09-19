// Fault-injects Tutorial03's own create*() failure branches - see
// Tutorial04FaultInjectionTest.cpp's header comment for the technique and
// why this all happens in one TEST() sharing one real device.

#include <gtest/gtest.h>

#include "vulkan_graphix/OperatingSystem.h"
#include "vulkan_graphix/Tutorial03.h"
#include "vulkan_graphix/VulkanFunctions.h"

#include "IntegrationTestCommon.h"

TEST(Tutorial03FaultInjectionTest,
     EachCreateMethodFailsWhenItsFirstVulkanCallFails) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                        "and X11 window)";
    }
    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("fault-injection-tutorial03"));

    vulkan_graphix::Tutorial03 tutorial;
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

    // createFramebuffers() fails when vkCreateFramebuffer fails.
    {
        auto real_fn = vulkan_graphix::vkCreateFramebuffer;
        vulkan_graphix::vkCreateFramebuffer =
                [](VkDevice,
                   const VkFramebufferCreateInfo*,
                   const VkAllocationCallbacks*,
                   VkFramebuffer*) -> VkResult {
            return VK_ERROR_INITIALIZATION_FAILED;
        };
        EXPECT_FALSE(tutorial.createFramebuffers());
        vulkan_graphix::vkCreateFramebuffer = real_fn;
    }

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

    // createSemaphores() fails when vkCreateSemaphore fails.
    {
        auto real_fn = vulkan_graphix::vkCreateSemaphore;
        vulkan_graphix::vkCreateSemaphore = [](VkDevice,
                                               const VkSemaphoreCreateInfo*,
                                               const VkAllocationCallbacks*,
                                               VkSemaphore*) -> VkResult {
            return VK_ERROR_INITIALIZATION_FAILED;
        };
        EXPECT_FALSE(tutorial.createSemaphores());
        vulkan_graphix::vkCreateSemaphore = real_fn;
    }

    // createCommandBuffers() fails when its command pool fails.
    {
        auto real_fn = vulkan_graphix::vkCreateCommandPool;
        vulkan_graphix::vkCreateCommandPool =
                [](VkDevice,
                   const VkCommandPoolCreateInfo*,
                   const VkAllocationCallbacks*,
                   VkCommandPool*) -> VkResult {
            return VK_ERROR_INITIALIZATION_FAILED;
        };
        EXPECT_FALSE(tutorial.createCommandBuffers());
        vulkan_graphix::vkCreateCommandPool = real_fn;
    }
}

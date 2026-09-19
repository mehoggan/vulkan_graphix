// Fault-injects Tutorial10's own create*() failure branches - see
// Tutorial04FaultInjectionTest.cpp's header comment for the technique and
// why this all happens in one TEST() sharing one real device.

#include <gtest/gtest.h>

#include "vulkan_graphix/OperatingSystem.h"
#include "vulkan_graphix/Tutorial10.h"
#include "vulkan_graphix/VulkanFunctions.h"

#include "IntegrationTestCommon.h"

TEST(Tutorial10FaultInjectionTest,
     EachCreateMethodFailsWhenItsFirstVulkanCallFails) {
    if (!vulkan_graphix::test::hasDisplay()) {
        GTEST_SKIP() << "No DISPLAY - skipping (needs a live Vulkan driver "
                        "and X11 window)";
    }
    vulkan_graphix::os::Window window;
    ASSERT_TRUE(window.create("fault-injection-tutorial10"));

    vulkan_graphix::Tutorial10 tutorial;
    ASSERT_TRUE(tutorial.prepareVulkan(window.getParameters()));

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
    ASSERT_TRUE(tutorial.createRenderingResources());

    // createStagingBuffer()/createUniformBuffer()/createCurveVertexBuffer()/
    // createControlPolygonVertexBuffer() all route through the shared
    // createBuffer() helper, whose first call is vkCreateBuffer - fault it
    // once per public entry point, since coverage tracks each call site's
    // own `if (!createBuffer(...))` check.
    {
        auto real_fn = vulkan_graphix::vkCreateBuffer;
        vulkan_graphix::vkCreateBuffer = [](VkDevice,
                                            const VkBufferCreateInfo*,
                                            const VkAllocationCallbacks*,
                                            VkBuffer*) -> VkResult {
            return VK_ERROR_INITIALIZATION_FAILED;
        };
        EXPECT_FALSE(tutorial.createStagingBuffer());
        EXPECT_FALSE(tutorial.createUniformBuffer());
        EXPECT_FALSE(tutorial.createCurveVertexBuffer());
        EXPECT_FALSE(tutorial.createControlPolygonVertexBuffer());
        vulkan_graphix::vkCreateBuffer = real_fn;
    }
    ASSERT_TRUE(tutorial.createStagingBuffer());

    // createDepthResources() fails when its underlying image fails to
    // create.
    {
        auto real_fn = vulkan_graphix::vkCreateImage;
        vulkan_graphix::vkCreateImage = [](VkDevice,
                                           const VkImageCreateInfo*,
                                           const VkAllocationCallbacks*,
                                           VkImage*) -> VkResult {
            return VK_ERROR_INITIALIZATION_FAILED;
        };
        EXPECT_FALSE(tutorial.createDepthResources());
        vulkan_graphix::vkCreateImage = real_fn;
    }

    // createDescriptorSetLayout() fails when vkCreateDescriptorSetLayout
    // fails.
    {
        auto real_fn = vulkan_graphix::vkCreateDescriptorSetLayout;
        vulkan_graphix::vkCreateDescriptorSetLayout =
                [](VkDevice,
                   const VkDescriptorSetLayoutCreateInfo*,
                   const VkAllocationCallbacks*,
                   VkDescriptorSetLayout*) -> VkResult {
            return VK_ERROR_INITIALIZATION_FAILED;
        };
        EXPECT_FALSE(tutorial.createDescriptorSetLayout());
        vulkan_graphix::vkCreateDescriptorSetLayout = real_fn;
    }
    ASSERT_TRUE(tutorial.createDescriptorSetLayout());

    // createDescriptorPool() fails when vkCreateDescriptorPool fails.
    {
        auto real_fn = vulkan_graphix::vkCreateDescriptorPool;
        vulkan_graphix::vkCreateDescriptorPool =
                [](VkDevice,
                   const VkDescriptorPoolCreateInfo*,
                   const VkAllocationCallbacks*,
                   VkDescriptorPool*) -> VkResult {
            return VK_ERROR_INITIALIZATION_FAILED;
        };
        EXPECT_FALSE(tutorial.createDescriptorPool());
        vulkan_graphix::vkCreateDescriptorPool = real_fn;
    }

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

    // createPipelineLayout() fails when vkCreatePipelineLayout fails.
    {
        auto real_fn = vulkan_graphix::vkCreatePipelineLayout;
        vulkan_graphix::vkCreatePipelineLayout =
                [](VkDevice,
                   const VkPipelineLayoutCreateInfo*,
                   const VkAllocationCallbacks*,
                   VkPipelineLayout*) -> VkResult {
            return VK_ERROR_INITIALIZATION_FAILED;
        };
        EXPECT_FALSE(tutorial.createPipelineLayout());
        vulkan_graphix::vkCreatePipelineLayout = real_fn;
    }

    // createLinePipeline() fails when its first shader module fails to
    // build.
    {
        auto real_fn = vulkan_graphix::vkCreateShaderModule;
        vulkan_graphix::vkCreateShaderModule =
                [](VkDevice,
                   const VkShaderModuleCreateInfo*,
                   const VkAllocationCallbacks*,
                   VkShaderModule*) -> VkResult {
            return VK_ERROR_INITIALIZATION_FAILED;
        };
        EXPECT_FALSE(tutorial.createLinePipeline());
        vulkan_graphix::vkCreateShaderModule = real_fn;
    }
}

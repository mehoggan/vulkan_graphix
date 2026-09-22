#include "VulkanProjectilePilot.h"
#include "vulkan_graphix/TutorialBase.h"

// Standalone proof-of-pattern pilot #3 for the vulkan_earth OpenGL ->
// Vulkan migration - see VulkanProjectilePilot.h. Not wired into the game;
// run directly (./vk_projectile_pilot_runner) to see the real tank-shell
// mesh, drag with the mouse to orbit around it.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> pilot =
            std::make_shared<vulkan_graphix::VulkanProjectilePilot>();

    if (!window.create("vulkan_earth pilot - Vulkan projectile")) {
        return -1;
    }

    if (!pilot->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::VulkanProjectilePilot> projectile_pilot =
            std::dynamic_pointer_cast<vulkan_graphix::VulkanProjectilePilot>(
                    pilot);

    if (!projectile_pilot->createRenderingResources()) {
        return -1;
    }
    if (!projectile_pilot->createStagingBuffer()) {
        return -1;
    }
    if (!projectile_pilot->createTexture()) {
        return -1;
    }
    if (!projectile_pilot->createUniformBuffer()) {
        return -1;
    }
    if (!projectile_pilot->createDescriptorSetLayout()) {
        return -1;
    }
    if (!projectile_pilot->createDescriptorPool()) {
        return -1;
    }
    if (!projectile_pilot->allocateDescriptorSet()) {
        return -1;
    }
    if (!projectile_pilot->updateDescriptorSet()) {
        return -1;
    }
    if (!projectile_pilot->createRenderPass()) {
        return -1;
    }
    if (!projectile_pilot->createPipelineLayout()) {
        return -1;
    }
    if (!projectile_pilot->createPipeline()) {
        return -1;
    }
    if (!projectile_pilot->createVertexBuffer()) {
        return -1;
    }

    if (!window.renderingLoop(*pilot)) {
        return -1;
    }

    return 0;
}

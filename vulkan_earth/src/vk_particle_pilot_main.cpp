#include "VulkanParticlePilot.h"
#include "vulkan_graphix/TutorialBase.h"

// Standalone proof-of-pattern pilot #4 for the vulkan_earth OpenGL ->
// Vulkan migration - see VulkanParticlePilot.h. Not wired into the game;
// run directly (./vk_particle_pilot_runner) to see the translucent
// particle-effect sphere, drag with the mouse to orbit around it.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> pilot =
            std::make_shared<vulkan_graphix::VulkanParticlePilot>();

    if (!window.create("vulkan_earth pilot - Vulkan particle")) {
        return -1;
    }

    if (!pilot->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::VulkanParticlePilot> particle_pilot =
            std::dynamic_pointer_cast<vulkan_graphix::VulkanParticlePilot>(
                    pilot);

    if (!particle_pilot->createRenderingResources()) {
        return -1;
    }
    if (!particle_pilot->createStagingBuffer()) {
        return -1;
    }
    if (!particle_pilot->createUniformBuffer()) {
        return -1;
    }
    if (!particle_pilot->createDescriptorSetLayout()) {
        return -1;
    }
    if (!particle_pilot->createDescriptorPool()) {
        return -1;
    }
    if (!particle_pilot->allocateDescriptorSet()) {
        return -1;
    }
    if (!particle_pilot->updateDescriptorSet()) {
        return -1;
    }
    if (!particle_pilot->createRenderPass()) {
        return -1;
    }
    if (!particle_pilot->createPipelineLayout()) {
        return -1;
    }
    if (!particle_pilot->createPipeline()) {
        return -1;
    }
    if (!particle_pilot->createVertexBuffer()) {
        return -1;
    }
    if (!particle_pilot->createIndexBuffer()) {
        return -1;
    }

    if (!window.renderingLoop(*pilot)) {
        return -1;
    }

    return 0;
}

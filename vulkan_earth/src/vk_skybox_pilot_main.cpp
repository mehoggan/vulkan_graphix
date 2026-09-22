#include "VulkanSkyboxPilot.h"
#include "vulkan_graphix/TutorialBase.h"

// Standalone proof-of-pattern pilot for the vulkan_earth OpenGL -> Vulkan
// migration - see VulkanSkyboxPilot.h. Not wired into the game; run
// directly (./vk_skybox_pilot_runner) to see the textured cube, drag with
// the mouse to orbit around/into it.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> pilot =
            std::make_shared<vulkan_graphix::VulkanSkyboxPilot>();

    if (!window.create("vulkan_earth pilot - Vulkan skybox")) {
        return -1;
    }

    if (!pilot->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::VulkanSkyboxPilot> skybox_pilot =
            std::dynamic_pointer_cast<vulkan_graphix::VulkanSkyboxPilot>(
                    pilot);

    if (!skybox_pilot->createRenderingResources()) {
        return -1;
    }
    if (!skybox_pilot->createStagingBuffer()) {
        return -1;
    }
    if (!skybox_pilot->createTexture()) {
        return -1;
    }
    if (!skybox_pilot->createUniformBuffer()) {
        return -1;
    }
    if (!skybox_pilot->createDescriptorSetLayout()) {
        return -1;
    }
    if (!skybox_pilot->createDescriptorPool()) {
        return -1;
    }
    if (!skybox_pilot->allocateDescriptorSet()) {
        return -1;
    }
    if (!skybox_pilot->updateDescriptorSet()) {
        return -1;
    }
    if (!skybox_pilot->createRenderPass()) {
        return -1;
    }
    if (!skybox_pilot->createPipelineLayout()) {
        return -1;
    }
    if (!skybox_pilot->createPipeline()) {
        return -1;
    }
    if (!skybox_pilot->createVertexBuffer()) {
        return -1;
    }
    if (!skybox_pilot->createIndexBuffer()) {
        return -1;
    }

    if (!window.renderingLoop(*pilot)) {
        return -1;
    }

    return 0;
}

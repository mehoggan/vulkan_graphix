#include "VulkanTerrainPilot.h"
#include "vulkan_graphix/TutorialBase.h"

// Standalone proof-of-pattern pilot #2 for the vulkan_earth OpenGL ->
// Vulkan migration - see VulkanTerrainPilot.h. Not wired into the game;
// run directly (./vk_terrain_pilot_runner) to see the generated hills,
// drag with the mouse to orbit around them.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> pilot =
            std::make_shared<vulkan_graphix::VulkanTerrainPilot>();

    if (!window.create("vulkan_earth pilot - Vulkan terrain")) {
        return -1;
    }

    if (!pilot->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::VulkanTerrainPilot> terrain_pilot =
            std::dynamic_pointer_cast<vulkan_graphix::VulkanTerrainPilot>(
                    pilot);

    if (!terrain_pilot->createRenderingResources()) {
        return -1;
    }
    if (!terrain_pilot->createStagingBuffer()) {
        return -1;
    }
    if (!terrain_pilot->createDepthResources()) {
        return -1;
    }
    if (!terrain_pilot->createTexture()) {
        return -1;
    }
    if (!terrain_pilot->createUniformBuffer()) {
        return -1;
    }
    if (!terrain_pilot->createDescriptorSetLayout()) {
        return -1;
    }
    if (!terrain_pilot->createDescriptorPool()) {
        return -1;
    }
    if (!terrain_pilot->allocateDescriptorSet()) {
        return -1;
    }
    if (!terrain_pilot->updateDescriptorSet()) {
        return -1;
    }
    if (!terrain_pilot->createRenderPass()) {
        return -1;
    }
    if (!terrain_pilot->createPipelineLayout()) {
        return -1;
    }
    if (!terrain_pilot->createPipeline()) {
        return -1;
    }
    if (!terrain_pilot->createVertexBuffer()) {
        return -1;
    }

    if (!window.renderingLoop(*pilot)) {
        return -1;
    }

    return 0;
}

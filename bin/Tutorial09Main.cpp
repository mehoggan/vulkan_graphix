#include "vulkan_graphix/Tutorial09.h"
#include "vulkan_graphix/TutorialBase.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial09>();

    // Window creation
    if (!window.create("09 - Tessellated Terrain")) {
        return -1;
    }

    // Vulkan preparations and initialization
    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial09> tutorial09 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial09>(tutorial);

    // Tutorial 09
    if (!tutorial09->createRenderingResources()) {
        return -1;
    }
    if (!tutorial09->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial09->createDepthResources()) {
        return -1;
    }
    if (!tutorial09->createTexture()) {
        return -1;
    }
    if (!tutorial09->createUniformBuffer()) {
        return -1;
    }
    if (!tutorial09->createDescriptorSetLayout()) {
        return -1;
    }
    if (!tutorial09->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial09->allocateDescriptorSet()) {
        return -1;
    }
    if (!tutorial09->updateDescriptorSet()) {
        return -1;
    }
    if (!tutorial09->createRenderPass()) {
        return -1;
    }
    if (!tutorial09->createPipelineLayout()) {
        return -1;
    }
    if (!tutorial09->createPipeline()) {
        return -1;
    }
    if (!tutorial09->createVertexBuffer()) {
        return -1;
    }
    if (!tutorial09->createIndexBuffer()) {
        return -1;
    }

    // Rendering loop
    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}

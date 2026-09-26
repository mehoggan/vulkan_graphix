#include <cstdlib>

#include "vulkan_graphix/Tutorial09.h"
#include "vulkan_graphix/TutorialBase.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial09>();

    // Window creation
    if (!window.create("09 - Tessellated Terrain")) {
        return EXIT_FAILURE;
    }

    // Vulkan preparations and initialization
    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial09> tutorial09 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial09>(tutorial);

    // Tutorial 09
    if (!tutorial09->createRenderingResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial09->createStagingBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial09->createDepthResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial09->createTexture()) {
        return EXIT_FAILURE;
    }
    if (!tutorial09->createUniformBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial09->createDescriptorSetLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial09->createDescriptorPool()) {
        return EXIT_FAILURE;
    }
    if (!tutorial09->allocateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial09->updateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial09->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial09->createPipelineLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial09->createPipeline()) {
        return EXIT_FAILURE;
    }
    if (!tutorial09->createVertexBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial09->createIndexBuffer()) {
        return EXIT_FAILURE;
    }

    // Rendering loop
    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

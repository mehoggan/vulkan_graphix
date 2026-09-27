#include <cstdlib>

#include "vulkan_graphix/Tutorial06.h"
#include "vulkan_graphix/TutorialBase.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial06>();

    // Window creation
    if (!window.create("06 - Descriptor Sets")) {
        return EXIT_FAILURE;
    }

    // Vulkan preparations and initialization
    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial06> tutorial06 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial06>(tutorial);

    // Tutorial 06
    if (!tutorial06->createRenderingResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial06->createStagingBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial06->createTexture()) {
        return EXIT_FAILURE;
    }
    if (!tutorial06->createDescriptorSetLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial06->createDescriptorPool()) {
        return EXIT_FAILURE;
    }
    if (!tutorial06->allocateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial06->updateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial06->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial06->createPipelineLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial06->createPipeline()) {
        return EXIT_FAILURE;
    }
    if (!tutorial06->createVertexBuffer()) {
        return EXIT_FAILURE;
    }

    // Rendering loop
    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

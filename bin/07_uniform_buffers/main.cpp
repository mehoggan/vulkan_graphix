#include <cstdlib>

#include "vulkan_graphix/Tutorial07.h"
#include "vulkan_graphix/TutorialBase.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial07>();

    // Window creation
    if (!window.create("07 - Uniform Buffers")) {
        return EXIT_FAILURE;
    }

    // Vulkan preparations and initialization
    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial07> tutorial07 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial07>(tutorial);

    // Tutorial 07
    if (!tutorial07->createRenderingResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial07->createStagingBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial07->createTexture()) {
        return EXIT_FAILURE;
    }
    if (!tutorial07->createUniformBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial07->createDescriptorSetLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial07->createDescriptorPool()) {
        return EXIT_FAILURE;
    }
    if (!tutorial07->allocateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial07->updateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial07->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial07->createPipelineLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial07->createPipeline()) {
        return EXIT_FAILURE;
    }
    if (!tutorial07->createVertexBuffer()) {
        return EXIT_FAILURE;
    }

    // Rendering loop
    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

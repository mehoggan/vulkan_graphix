#include <cstdlib>

#include "vulkan_graphix/Tutorial08.h"
#include "vulkan_graphix/TutorialBase.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial08>();

    // Window creation
    if (!window.create("08 - Phong Sphere")) {
        return EXIT_FAILURE;
    }

    // Vulkan preparations and initialization
    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial08> tutorial08 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial08>(tutorial);

    // Tutorial 08
    if (!tutorial08->createRenderingResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial08->createStagingBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial08->createDepthResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial08->createUniformBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial08->createDescriptorSetLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial08->createDescriptorPool()) {
        return EXIT_FAILURE;
    }
    if (!tutorial08->allocateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial08->updateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial08->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial08->createPipelineLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial08->createPipeline()) {
        return EXIT_FAILURE;
    }
    if (!tutorial08->createVertexBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial08->createIndexBuffer()) {
        return EXIT_FAILURE;
    }

    // Rendering loop
    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

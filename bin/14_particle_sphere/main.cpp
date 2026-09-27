#include <cstdlib>

#include "vulkan_graphix/Tutorial14.h"
#include "vulkan_graphix/TutorialBase.h"

// Translucent particle-effect sphere ported from vulkan_earth's
// Particle::render() (see include/vulkan_graphix/Tutorial14.h) - drag with
// the mouse to orbit around it.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial14>();

    if (!window.create("14 - Particle Sphere")) {
        return EXIT_FAILURE;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial14> tutorial14 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial14>(tutorial);

    if (!tutorial14->createRenderingResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial14->createStagingBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial14->createUniformBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial14->createDescriptorSetLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial14->createDescriptorPool()) {
        return EXIT_FAILURE;
    }
    if (!tutorial14->allocateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial14->updateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial14->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial14->createPipelineLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial14->createPipeline()) {
        return EXIT_FAILURE;
    }
    if (!tutorial14->createVertexBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial14->createIndexBuffer()) {
        return EXIT_FAILURE;
    }

    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

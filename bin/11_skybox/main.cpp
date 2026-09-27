#include <cstdlib>

#include "vulkan_graphix/Tutorial11.h"
#include "vulkan_graphix/TutorialBase.h"

// Indexed, textured cube ported from vulkan_earth's SkyboxFactory geometry
// (see include/vulkan_graphix/Tutorial11.h) - drag with the mouse to orbit
// around/into it.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial11>();

    if (!window.create("11 - Skybox")) {
        return EXIT_FAILURE;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial11> tutorial11 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial11>(tutorial);

    if (!tutorial11->createRenderingResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial11->createStagingBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial11->createTexture()) {
        return EXIT_FAILURE;
    }
    if (!tutorial11->createUniformBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial11->createDescriptorSetLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial11->createDescriptorPool()) {
        return EXIT_FAILURE;
    }
    if (!tutorial11->allocateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial11->updateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial11->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial11->createPipelineLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial11->createPipeline()) {
        return EXIT_FAILURE;
    }
    if (!tutorial11->createVertexBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial11->createIndexBuffer()) {
        return EXIT_FAILURE;
    }

    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

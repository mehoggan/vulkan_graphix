#include <cstdlib>

#include "vulkan_graphix/Tutorial12.h"
#include "vulkan_graphix/TutorialBase.h"

// Generated terrain ported from vulkan_earth's TerrainMaker height-field
// algorithm (see include/vulkan_graphix/Tutorial12.h and
// TerrainGenerator.h) - drag with the mouse to orbit around it.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial12>();

    if (!window.create("12 - Generated Terrain")) {
        return EXIT_FAILURE;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial12> tutorial12 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial12>(tutorial);

    if (!tutorial12->createRenderingResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial12->createStagingBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial12->createDepthResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial12->createTexture()) {
        return EXIT_FAILURE;
    }
    if (!tutorial12->createUniformBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial12->createDescriptorSetLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial12->createDescriptorPool()) {
        return EXIT_FAILURE;
    }
    if (!tutorial12->allocateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial12->updateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial12->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial12->createPipelineLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial12->createPipeline()) {
        return EXIT_FAILURE;
    }
    if (!tutorial12->createVertexBuffer()) {
        return EXIT_FAILURE;
    }

    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

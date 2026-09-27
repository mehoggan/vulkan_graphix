#include <cstdlib>

#include "vulkan_graphix/Tutorial20.h"
#include "vulkan_graphix/TutorialBase.h"

// Real vulkan_earth Effects: live smoke/acid/float particle streams (each
// using its own real per-type update formula), a growing/fading/color-
// cycling explosion using a real weapon's real explosion colors, and the
// real translucent shield sphere - see include/vulkan_graphix/
// Tutorial20.h. Drag with the mouse to orbit around the scene.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial20>();

    if (!window.create("20 - Effects")) {
        return EXIT_FAILURE;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial20> tutorial20 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial20>(tutorial);

    if (!tutorial20->createRenderingResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial20->createStagingBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial20->createUniformBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial20->createDescriptorSetLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial20->createDescriptorPool()) {
        return EXIT_FAILURE;
    }
    if (!tutorial20->allocateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial20->updateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial20->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial20->createPipelineLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial20->createPipeline()) {
        return EXIT_FAILURE;
    }
    if (!tutorial20->createVertexBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial20->createIndexBuffer()) {
        return EXIT_FAILURE;
    }

    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#include <cstdlib>

#include "vulkan_graphix/Tutorial17.h"
#include "vulkan_graphix/TutorialBase.h"

// A titled grid of the 8 real vulkan_earth items (see vulkan_earth/src/
// ItemXxx.cpp), ported from Inventory's own bevel-panel/icon-grid
// rendering - see include/vulkan_graphix/Tutorial17.h. Click an item to
// select it and read its real description.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial17>();

    if (!window.create("17 - Inventory")) {
        return EXIT_FAILURE;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial17> tutorial17 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial17>(tutorial);

    if (!tutorial17->createRenderingResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial17->createStagingBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial17->createFontAtlas()) {
        return EXIT_FAILURE;
    }
    if (!tutorial17->createIconAtlas()) {
        return EXIT_FAILURE;
    }
    if (!tutorial17->createUniformBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial17->createDescriptorSetLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial17->createDescriptorPool()) {
        return EXIT_FAILURE;
    }
    if (!tutorial17->allocateDescriptorSets()) {
        return EXIT_FAILURE;
    }
    if (!tutorial17->updateDescriptorSets()) {
        return EXIT_FAILURE;
    }
    if (!tutorial17->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial17->createPipelineLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial17->createPipeline()) {
        return EXIT_FAILURE;
    }
    if (!tutorial17->createVertexBuffer()) {
        return EXIT_FAILURE;
    }

    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

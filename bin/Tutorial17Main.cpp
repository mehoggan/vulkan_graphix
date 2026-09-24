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
        return -1;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial17> tutorial17 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial17>(tutorial);

    if (!tutorial17->createRenderingResources()) {
        return -1;
    }
    if (!tutorial17->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial17->createFontAtlas()) {
        return -1;
    }
    if (!tutorial17->createIconAtlas()) {
        return -1;
    }
    if (!tutorial17->createUniformBuffer()) {
        return -1;
    }
    if (!tutorial17->createDescriptorSetLayout()) {
        return -1;
    }
    if (!tutorial17->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial17->allocateDescriptorSets()) {
        return -1;
    }
    if (!tutorial17->updateDescriptorSets()) {
        return -1;
    }
    if (!tutorial17->createRenderPass()) {
        return -1;
    }
    if (!tutorial17->createPipelineLayout()) {
        return -1;
    }
    if (!tutorial17->createPipeline()) {
        return -1;
    }
    if (!tutorial17->createVertexBuffer()) {
        return -1;
    }

    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}

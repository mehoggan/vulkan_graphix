#include "vulkan_graphix/Tutorial15.h"
#include "vulkan_graphix/TutorialBase.h"

// A text title and one clickable button with a text label - demonstrates
// the two primitives vulkan_earth's whole UI is built from (see
// include/vulkan_graphix/Tutorial15.h, BitmapFont.h, UiGeometry.h).
// Click the button; its label and bevel update on the next frame.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial15>();

    if (!window.create("15 - Text & UI")) {
        return -1;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial15> tutorial15 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial15>(tutorial);

    if (!tutorial15->createRenderingResources()) {
        return -1;
    }
    if (!tutorial15->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial15->createFontAtlas()) {
        return -1;
    }
    if (!tutorial15->createUniformBuffer()) {
        return -1;
    }
    if (!tutorial15->createDescriptorSetLayout()) {
        return -1;
    }
    if (!tutorial15->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial15->allocateDescriptorSet()) {
        return -1;
    }
    if (!tutorial15->updateDescriptorSet()) {
        return -1;
    }
    if (!tutorial15->createRenderPass()) {
        return -1;
    }
    if (!tutorial15->createPipelineLayout()) {
        return -1;
    }
    if (!tutorial15->createPipeline()) {
        return -1;
    }
    if (!tutorial15->createVertexBuffer()) {
        return -1;
    }

    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}

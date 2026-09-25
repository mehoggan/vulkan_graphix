#include "vulkan_graphix/Tutorial22.h"
#include "vulkan_graphix/TutorialBase.h"

// Real vulkan_earth GameState menu layer: the identical 5-quad bevel-
// panel background every real menu screen (MainMenu/ReadyMenu/ShopMenu/
// SubMenu*) opens with, plus ReadyMenu's own real technique - a live
// rotating 3D tank preview rendered into a scissored sub-region of the
// screen - see include/vulkan_graphix/Tutorial22.h. Click the button to
// see the click counter update.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial22>();

    if (!window.create("22 - GameState Menu")) {
        return -1;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial22> tutorial22 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial22>(tutorial);

    if (!tutorial22->createRenderingResources()) {
        return -1;
    }
    if (!tutorial22->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial22->createDepthResources()) {
        return -1;
    }
    if (!tutorial22->createTankTexture()) {
        return -1;
    }
    if (!tutorial22->createFontAtlas()) {
        return -1;
    }
    if (!tutorial22->createUniformBuffers()) {
        return -1;
    }
    if (!tutorial22->createDescriptorSetLayouts()) {
        return -1;
    }
    if (!tutorial22->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial22->allocateDescriptorSets()) {
        return -1;
    }
    if (!tutorial22->updateDescriptorSets()) {
        return -1;
    }
    if (!tutorial22->createRenderPass()) {
        return -1;
    }
    if (!tutorial22->createPipelineLayouts()) {
        return -1;
    }
    if (!tutorial22->createPipelines()) {
        return -1;
    }
    if (!tutorial22->createTankVertexBuffers()) {
        return -1;
    }
    if (!tutorial22->createPanelVertexBuffer()) {
        return -1;
    }

    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}

#include "vulkan_graphix/Tutorial18.h"
#include "vulkan_graphix/TutorialBase.h"

// Two real Hellfire tanks positioned via Tank::setTankPos()'s real
// hierarchical composition (see include/vulkan_graphix/Tutorial18.h),
// tinted per-player, with a HUD overlay using the real
// GameState::drawHUD() health/power color ramps. Drag with the mouse to
// orbit around both.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial18>();

    if (!window.create("18 - Player")) {
        return -1;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial18> tutorial18 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial18>(tutorial);

    if (!tutorial18->createRenderingResources()) {
        return -1;
    }
    if (!tutorial18->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial18->createDepthResources()) {
        return -1;
    }
    if (!tutorial18->createTankTexture()) {
        return -1;
    }
    if (!tutorial18->createFontAtlas()) {
        return -1;
    }
    if (!tutorial18->createUniformBuffers()) {
        return -1;
    }
    if (!tutorial18->createDescriptorSetLayouts()) {
        return -1;
    }
    if (!tutorial18->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial18->allocateDescriptorSets()) {
        return -1;
    }
    if (!tutorial18->updateDescriptorSets()) {
        return -1;
    }
    if (!tutorial18->createRenderPass()) {
        return -1;
    }
    if (!tutorial18->createPipelineLayouts()) {
        return -1;
    }
    if (!tutorial18->createPipelines()) {
        return -1;
    }
    if (!tutorial18->createTankVertexBuffers()) {
        return -1;
    }
    if (!tutorial18->createHudVertexBuffer()) {
        return -1;
    }

    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}

#include <cstdlib>

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
        return EXIT_FAILURE;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial18> tutorial18 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial18>(tutorial);

    if (!tutorial18->createRenderingResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial18->createStagingBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial18->createDepthResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial18->createTankTexture()) {
        return EXIT_FAILURE;
    }
    if (!tutorial18->createFontAtlas()) {
        return EXIT_FAILURE;
    }
    if (!tutorial18->createUniformBuffers()) {
        return EXIT_FAILURE;
    }
    if (!tutorial18->createDescriptorSetLayouts()) {
        return EXIT_FAILURE;
    }
    if (!tutorial18->createDescriptorPool()) {
        return EXIT_FAILURE;
    }
    if (!tutorial18->allocateDescriptorSets()) {
        return EXIT_FAILURE;
    }
    if (!tutorial18->updateDescriptorSets()) {
        return EXIT_FAILURE;
    }
    if (!tutorial18->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial18->createPipelineLayouts()) {
        return EXIT_FAILURE;
    }
    if (!tutorial18->createPipelines()) {
        return EXIT_FAILURE;
    }
    if (!tutorial18->createTankVertexBuffers()) {
        return EXIT_FAILURE;
    }
    if (!tutorial18->createHudVertexBuffer()) {
        return EXIT_FAILURE;
    }

    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#include "vulkan_graphix/Tutorial19.h"
#include "vulkan_graphix/TutorialBase.h"

// Three real, distinct projectile meshes (Default/Acid/BFB) each scaled
// by their own weapon's real scale value, demonstrating Projectile::
// draw()'s real per-weapon mesh dispatch, plus a grid of the 10 real
// shop-purchasable weapons - see include/vulkan_graphix/Tutorial19.h.
// Click a weapon cell to select it and read its real description; drag
// with the mouse to orbit the projectiles.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial19>();

    if (!window.create("19 - Weapon")) {
        return -1;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial19> tutorial19 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial19>(tutorial);

    if (!tutorial19->createRenderingResources()) {
        return -1;
    }
    if (!tutorial19->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial19->createProjectileTextures()) {
        return -1;
    }
    if (!tutorial19->createFontAtlas()) {
        return -1;
    }
    if (!tutorial19->createIconAtlas()) {
        return -1;
    }
    if (!tutorial19->createUniformBuffers()) {
        return -1;
    }
    if (!tutorial19->createDescriptorSetLayouts()) {
        return -1;
    }
    if (!tutorial19->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial19->allocateDescriptorSets()) {
        return -1;
    }
    if (!tutorial19->updateDescriptorSets()) {
        return -1;
    }
    if (!tutorial19->createRenderPass()) {
        return -1;
    }
    if (!tutorial19->createPipelineLayouts()) {
        return -1;
    }
    if (!tutorial19->createPipelines()) {
        return -1;
    }
    if (!tutorial19->createProjectileVertexBuffers()) {
        return -1;
    }
    if (!tutorial19->createGridVertexBuffer()) {
        return -1;
    }

    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}

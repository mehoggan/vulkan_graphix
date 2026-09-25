#include "vulkan_graphix/Tutorial21.h"
#include "vulkan_graphix/TutorialBase.h"

// Real vulkan_earth World/Camera integration: GameState::draw()'s own
// skybox -> terrain -> tank order, combining three previously-separate
// pilots (Tutorial11's skybox, Tutorial12's terrain, Tutorial16's tank)
// into one real scene, with the tank placed via a real
// TerrainGenerator::heightAt() query - see
// include/vulkan_graphix/Tutorial21.h. Drag with the mouse to orbit
// around the scene.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial21>();

    if (!window.create("21 - World")) {
        return -1;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial21> tutorial21 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial21>(tutorial);

    if (!tutorial21->createRenderingResources()) {
        return -1;
    }
    if (!tutorial21->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial21->createDepthResources()) {
        return -1;
    }
    if (!tutorial21->createTerrainTexture()) {
        return -1;
    }
    if (!tutorial21->createTankTexture()) {
        return -1;
    }
    if (!tutorial21->createSkyboxTexture()) {
        return -1;
    }
    if (!tutorial21->createUniformBuffers()) {
        return -1;
    }
    if (!tutorial21->createDescriptorSetLayouts()) {
        return -1;
    }
    if (!tutorial21->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial21->allocateDescriptorSets()) {
        return -1;
    }
    if (!tutorial21->updateDescriptorSets()) {
        return -1;
    }
    if (!tutorial21->createRenderPass()) {
        return -1;
    }
    if (!tutorial21->createPipelineLayouts()) {
        return -1;
    }
    if (!tutorial21->createPipelines()) {
        return -1;
    }
    if (!tutorial21->createTerrainVertexBuffer()) {
        return -1;
    }
    if (!tutorial21->createTankVertexBuffers()) {
        return -1;
    }
    if (!tutorial21->createSkyboxBuffers()) {
        return -1;
    }

    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}

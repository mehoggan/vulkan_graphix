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
        return -1;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial12> tutorial12 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial12>(tutorial);

    if (!tutorial12->createRenderingResources()) {
        return -1;
    }
    if (!tutorial12->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial12->createDepthResources()) {
        return -1;
    }
    if (!tutorial12->createTexture()) {
        return -1;
    }
    if (!tutorial12->createUniformBuffer()) {
        return -1;
    }
    if (!tutorial12->createDescriptorSetLayout()) {
        return -1;
    }
    if (!tutorial12->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial12->allocateDescriptorSet()) {
        return -1;
    }
    if (!tutorial12->updateDescriptorSet()) {
        return -1;
    }
    if (!tutorial12->createRenderPass()) {
        return -1;
    }
    if (!tutorial12->createPipelineLayout()) {
        return -1;
    }
    if (!tutorial12->createPipeline()) {
        return -1;
    }
    if (!tutorial12->createVertexBuffer()) {
        return -1;
    }

    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}

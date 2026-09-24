#include "vulkan_graphix/Tutorial16.h"
#include "vulkan_graphix/TutorialBase.h"

// The real "Hellfire" tank (vulkan_earth/src/TankB.cpp) assembled from its
// three mesh parts (body/head/turret), each parsed via the shared
// Tools::loadOglMeshData() and sharing one texture (TestImage.raw) - see
// include/vulkan_graphix/Tutorial16.h. Drag with the mouse to orbit
// around it.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial16>();

    if (!window.create("16 - Tank")) {
        return -1;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial16> tutorial16 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial16>(tutorial);

    if (!tutorial16->createRenderingResources()) {
        return -1;
    }
    if (!tutorial16->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial16->createTexture()) {
        return -1;
    }
    if (!tutorial16->createUniformBuffer()) {
        return -1;
    }
    if (!tutorial16->createDescriptorSetLayout()) {
        return -1;
    }
    if (!tutorial16->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial16->allocateDescriptorSet()) {
        return -1;
    }
    if (!tutorial16->updateDescriptorSet()) {
        return -1;
    }
    if (!tutorial16->createRenderPass()) {
        return -1;
    }
    if (!tutorial16->createPipelineLayout()) {
        return -1;
    }
    if (!tutorial16->createPipeline()) {
        return -1;
    }
    if (!tutorial16->createVertexBuffers()) {
        return -1;
    }

    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}

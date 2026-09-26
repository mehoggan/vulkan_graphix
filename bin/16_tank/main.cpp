#include <cstdlib>

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
        return EXIT_FAILURE;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial16> tutorial16 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial16>(tutorial);

    if (!tutorial16->createRenderingResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial16->createStagingBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial16->createTexture()) {
        return EXIT_FAILURE;
    }
    if (!tutorial16->createUniformBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial16->createDescriptorSetLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial16->createDescriptorPool()) {
        return EXIT_FAILURE;
    }
    if (!tutorial16->allocateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial16->updateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial16->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial16->createPipelineLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial16->createPipeline()) {
        return EXIT_FAILURE;
    }
    if (!tutorial16->createVertexBuffers()) {
        return EXIT_FAILURE;
    }

    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

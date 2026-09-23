#include "vulkan_graphix/Tutorial05.h"
#include "vulkan_graphix/TutorialBase.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial05>();

    // Window creation
    if (!window.create("05 - Staging Resources")) {
        return -1;
    }

    // Vulkan preparations and initialization
    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial05> tutorial05 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial05>(tutorial);

    // Tutorial 05
    if (!tutorial05->createRenderingResources()) {
        return -1;
    }
    if (!tutorial05->createRenderPass()) {
        return -1;
    }
    if (!tutorial05->createPipeline()) {
        return -1;
    }
    if (!tutorial05->createVertexBuffer()) {
        return -1;
    }
    if (!tutorial05->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial05->copyVertexData()) {
        return -1;
    }

    // Rendering loop
    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}

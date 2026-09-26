#include <cstdlib>

#include "vulkan_graphix/Tutorial02.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    vulkan_graphix::Tutorial02 tutorial02;

    // Window creation
    if (!window.create("02 - Swap chain")) {
        return EXIT_FAILURE;
    }

    // Vulkan preparations and initialization
    if (!tutorial02.prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }
    if (!tutorial02.createSwapChain()) {
        return EXIT_FAILURE;
    }
    if (!tutorial02.createCommandBuffers()) {
        return EXIT_FAILURE;
    }

    // Rendering loop
    if (!window.renderingLoop(tutorial02)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

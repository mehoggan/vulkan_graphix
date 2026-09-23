#include "vulkan_graphix/Tutorial01.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    vulkan_graphix::Tutorial01 tutorial01(true);

    // Window creation
    if (!window.create("01 - The Beginning")) {
        return -1;
    }

    // Vulkan preparations and initialization
    if (!tutorial01.prepareVulkan()) {
        return -1;
    }

    // Rendering loop
    if (!window.renderingLoop(tutorial01)) {
        return -1;
    }

    return 0;
}

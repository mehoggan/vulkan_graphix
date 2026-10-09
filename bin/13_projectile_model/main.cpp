#include <cstdlib>

#include "Tutorial13.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"

// Real tank-shell mesh, parsed from vulkan_earth's own projectileDefault.ogl
// via the shared Tools::loadOglMeshData() (see include/vulkan_graphix/
// Tutorial13.h) - drag with the mouse to orbit around it.
int main(int /*argc*/, char** /*argv*/) {
  vulkan_graphix::os::Window window;
  std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
      std::make_shared<vulkan_graphix::Tutorial13>();

  if (!window.create("13 - Projectile Model")) {
    return EXIT_FAILURE;
  }

  if (!tutorial->prepareVulkan(window.getParameters())) {
    return EXIT_FAILURE;
  }

  std::shared_ptr<vulkan_graphix::Tutorial13> tutorial13 =
      std::dynamic_pointer_cast<vulkan_graphix::Tutorial13>(tutorial);

  if (!tutorial13->createResources()) {
    return EXIT_FAILURE;
  }

  if (!window.renderingLoop(*tutorial)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

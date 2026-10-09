#include <cstdlib>

#include "Tutorial19.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"

// Three real, distinct projectile meshes (Default/Acid/BFB) each scaled
// by their own weapon's real scale value, demonstrating Projectile::
// draw()'s real per-weapon mesh dispatch, plus a grid of the 10 real
// shop-purchasable weapons - see include/vulkan_graphix/Tutorial/Tutorial19.h.
// Click a weapon cell to select it and read its real description; drag
// with the mouse to orbit the projectiles.
int main(int /*argc*/, char** /*argv*/) {
  vulkan_graphix::os::Window window;
  std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
      std::make_shared<vulkan_graphix::Tutorial19>();

  if (!window.create("19 - Weapon")) {
    return EXIT_FAILURE;
  }

  if (!tutorial->prepareVulkan(window.getParameters())) {
    return EXIT_FAILURE;
  }

  std::shared_ptr<vulkan_graphix::Tutorial19> tutorial19 =
      std::dynamic_pointer_cast<vulkan_graphix::Tutorial19>(tutorial);

  if (!tutorial19->createResources()) {
    return EXIT_FAILURE;
  }

  if (!window.renderingLoop(*tutorial)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

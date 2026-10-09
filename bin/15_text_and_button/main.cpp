#include <cstdlib>

#include "Tutorial15.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"

// A text title and one clickable button with a text label - demonstrates
// the two primitives vulkan_earth's whole UI is built from (see
// include/vulkan_graphix/Tutorial/Tutorial15.h, BitmapFont.h, UiGeometry.h).
// Click the button; its label and bevel update on the next frame.
int main(int /*argc*/, char** /*argv*/) {
  vulkan_graphix::os::Window window;
  std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
      std::make_shared<vulkan_graphix::Tutorial15>();

  if (!window.create("15 - Text & UI")) {
    return EXIT_FAILURE;
  }

  if (!tutorial->prepareVulkan(window.getParameters())) {
    return EXIT_FAILURE;
  }

  std::shared_ptr<vulkan_graphix::Tutorial15> tutorial15 =
      std::dynamic_pointer_cast<vulkan_graphix::Tutorial15>(tutorial);

  if (!tutorial15->createResources()) {
    return EXIT_FAILURE;
  }

  if (!window.renderingLoop(*tutorial)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

# Shared: `TutorialBase` bring-up

[Index](../vulkan_pipeline_breakdown.md) · Used by Tutorial03–22 (and the
`vulkan_earth` game)

`TutorialBase::prepareVulkan()` (`lib/TutorialBase.cpp`) is the device and
swapchain setup that [Tutorial01](01_device_initialization.md) and
[Tutorial02](02_swap_chain.md) spell out inline. Every later tutorial calls
it first and starts its own pipeline work after it returns.

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef reused fill:#eceff1,stroke:#607d8b,stroke-dasharray:4 3,color:#263238
    classDef shared fill:#e3f2fd,stroke:#1565c0,color:#0d47a1
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    LIB["↩ Tutorial01<br/>loadVulkanLibrary · loadExportedEntryPoints<br/>loadGlobalLevelEntryPoints"]:::reused
    INST["<b>createInstance</b><br/>VK_KHR_surface + VK_KHR_xlib_surface<br/>+ VK_EXT_debug_utils when validation is on<br/>apiVersion 1.0 · setupDebugMessenger()"]:::shared
    ILEP["↩ Tutorial01<br/>loadInstanceLevelEntryPoints"]:::reused
    SURF["↩ Tutorial02<br/>createPresentationSurface<br/>vkCreateXlibSurfaceKHR"]:::reused
    DEV["<b>createDevice</b><br/>graphics + present queue families<br/>VK_KHR_swapchain<br/>enables wideLines and fillModeNonSolid<br/>when the GPU supports them"]:::shared
    DLEP["↩ Tutorial01<br/>loadDeviceLevelEntryPoints"]:::reused
    Q["↩ Tutorial02<br/>getDeviceQueue<br/>graphics queue + present queue"]:::reused
    SC["<b>createSwapChain</b><br/>format R8G8B8A8_UNORM if offered<br/>present mode MAILBOX → IMMEDIATE → FIFO<br/>usage COLOR_ATTACHMENT (+ TRANSFER_SRC)<br/>vkCreateSwapchainKHR · vkGetSwapchainImagesKHR"]:::shared
    IV["<b>createSwapChainImageViews</b><br/>vkCreateImageView per swapchain image<br/>sets m_can_render"]:::shared
    DONE(["Tutorial's own setup starts here"]):::frame

    LIB --> INST --> ILEP --> SURF --> DEV --> DLEP --> Q --> SC --> IV --> DONE

    RESIZE["<b>onWindowSizeChanged</b><br/>vkDeviceWaitIdle → childClear()<br/>→ createSwapChain() → childOnWindowSizeChanged()"]:::shared
    DONE -. "window resized or<br/>VK_ERROR_OUT_OF_DATE_KHR" .-> RESIZE
    RESIZE -.-> SC

    click LIB "./01_device_initialization.md" "Tutorial01"
    click ILEP "./01_device_initialization.md" "Tutorial01"
    click DLEP "./01_device_initialization.md" "Tutorial01"
    click SURF "./02_swap_chain.md" "Tutorial02"
    click Q "./02_swap_chain.md" "Tutorial02"
```

**Linked steps:** library and entry-point loading come from
[Tutorial01](01_device_initialization.md); the surface and the two queues
come from [Tutorial02](02_swap_chain.md).

## What the base adds over Tutorial02

- **Swapchain image views.** Tutorial02 only clears swapchain images with
  transfer commands, so it never makes views. Framebuffers need them, so
  the base creates one per image.
- **No sync objects.** Semaphores, fences, and command buffers are left to
  each tutorial (Tutorial03–10) or to
  [`FrameLoop`](vulkancommon.md) (Tutorial11–22).
- **Resize protocol.** A tutorial overrides `childClear()` (destroy
  everything that depends on the swapchain) and `childOnWindowSizeChanged()`
  (build it again). The base recreates the swapchain in between, passing
  the old one as `oldSwapchain`.

## Spec links

[`vkCreateInstance`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateInstance.html) ·
[`vkCreateXlibSurfaceKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateXlibSurfaceKHR.html) ·
[`vkCreateDevice`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateDevice.html) ·
[`vkCreateSwapchainKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateSwapchainKHR.html) ·
[`vkCreateImageView`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateImageView.html)

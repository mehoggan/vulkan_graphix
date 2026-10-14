# Tutorial02 — Swapchain and a cleared window

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial01](01_device_initialization.md) · Next:
[Tutorial03](03_first_triangle.md)

Opens an X11 window, presents to it, and fills each swapchain image with a
solid color using a transfer command. There is no render pass, pipeline,
or shader yet.

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef reused fill:#eceff1,stroke:#607d8b,stroke-dasharray:4 3,color:#263238
    classDef changed fill:#fff3e0,stroke:#ef6c00,color:#3e2723
    classDef new fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    LOAD["↩ Tutorial01<br/>loadVulkanLibrary · loadExportedEntryPoints<br/>loadGlobalLevelEntryPoints"]:::reused
    INST["<b>createInstance</b> (changed)<br/>+ VK_KHR_surface + VK_KHR_xlib_surface<br/>checked with vkEnumerateInstanceExtensionProperties"]:::changed
    ILEP["↩ Tutorial01<br/>loadInstanceLevelEntryPoints"]:::reused
    SURF["<b>createPresentationSurface</b><br/>vkCreateXlibSurfaceKHR"]:::new
    DEV["<b>createDevice</b> (changed)<br/>vkGetPhysicalDeviceSurfaceSupportKHR<br/>graphics family + present family<br/>+ VK_KHR_swapchain"]:::changed
    DLEP["↩ Tutorial01<br/>loadDeviceLevelEntryPoints"]:::reused
    Q["<b>getDeviceQueue</b> (changed)<br/>graphics queue + present queue"]:::changed
    SEM["<b>createSemaphores</b><br/>1 image-available semaphore<br/>1 fence, created signaled"]:::new
    SC["<b>createSwapChain</b><br/>surface caps / formats / present modes<br/>vkCreateSwapchainKHR<br/>usage includes TRANSFER_DST · no image views"]:::new
    CB["<b>createCommandBuffers</b><br/>pool on the <i>present</i> family<br/>1 command buffer per swapchain image<br/>1 rendering-finished semaphore per image"]:::new
    REC["<b>recordCommandBuffers</b> (once, SIMULTANEOUS_USE)<br/>barrier UNDEFINED → TRANSFER_DST<br/>vkCmdClearColorImage (1.0, 0.8, 0.4)<br/>barrier TRANSFER_DST → PRESENT_SRC"]:::new
    DRAW(["<b>draw()</b><br/>wait fence · vkAcquireNextImageKHR<br/>vkQueueSubmit on the present queue,<br/>waiting at TRANSFER stage<br/>vkQueuePresentKHR"]):::frame

    LOAD --> INST --> ILEP --> SURF --> DEV --> DLEP --> Q --> SEM --> SC --> CB --> REC --> DRAW

    click LOAD "./01_device_initialization.md" "Tutorial01"
    click ILEP "./01_device_initialization.md" "Tutorial01"
    click DLEP "./01_device_initialization.md" "Tutorial01"
```

**Linked steps:** loading and instance-level entry points come from
[Tutorial01](01_device_initialization.md).

## What's new

- A window surface and a device that can present to it, possibly from a
  different queue family than graphics.
- The swapchain, recreated by `onWindowSizeChanged()`.
- Pre-recorded command buffers, one per swapchain image.
- The clear is a transfer command. From Tutorial03 on, a render pass
  `loadOp` does it instead.

## Spec links

[`vkCreateXlibSurfaceKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateXlibSurfaceKHR.html) ·
[`vkGetPhysicalDeviceSurfaceSupportKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceSurfaceSupportKHR.html) ·
[`vkCreateSwapchainKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateSwapchainKHR.html) ·
[`vkCmdClearColorImage`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdClearColorImage.html) ·
[`vkAcquireNextImageKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkAcquireNextImageKHR.html) ·
[`vkQueuePresentKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkQueuePresentKHR.html)

Source: `bin/02_swap_chain/Tutorial02.cpp`

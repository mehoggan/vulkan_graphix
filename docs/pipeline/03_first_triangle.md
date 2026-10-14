# Tutorial03 — First triangle

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial02](02_swap_chain.md) via [TutorialBase](tutorialbase.md) ·
Next: [Tutorial04](04_vertex_attributes.md)

The first real graphics pipeline: a render pass, framebuffers, and SPIR-V
shaders. The vertex shader makes the triangle's three corners from
`gl_VertexIndex`, so there is no vertex buffer.

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef reused fill:#eceff1,stroke:#607d8b,stroke-dasharray:4 3,color:#263238
    classDef new fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    BASE["↩ TutorialBase<br/>prepareVulkan()<br/>instance → surface → device → swapchain<br/>+ swapchain image views"]:::reused
    RP["<b>createRenderPass</b><br/>1 color attachment: CLEAR / STORE<br/>UNDEFINED → PRESENT_SRC_KHR<br/>1 subpass · no dependencies"]:::new
    FB["<b>createFramebuffers</b><br/>1 per swapchain image view, made up front"]:::new
    PIPE["<b>createPipeline</b><br/>shader.03.vert/frag.spv · no vertex input<br/>TRIANGLE_LIST · cull BACK · CCW<br/>static viewport = swapchain extent<br/>empty pipeline layout · blending off"]:::new
    SEM["<b>createSemaphores</b><br/>1 image-available semaphore<br/>1 rendering-finished semaphore per image<br/>1 fence (signaled)"]:::new
    CB["<b>createCommandBuffers</b><br/>pool on the <i>graphics</i> family<br/>1 command buffer per swapchain image"]:::new
    REC["<b>recordCommandBuffers</b> (once)<br/>vkCmdBeginRenderPass (clear 1.0, 0.8, 0.4)<br/>vkCmdBindPipeline · vkCmdDraw(3)<br/>vkCmdEndRenderPass<br/>+ queue-ownership barriers when the<br/>graphics and present queues differ"]:::new
    DRAW(["<b>draw()</b><br/>wait fence · acquire<br/>submit to graphics queue,<br/>waiting at COLOR_ATTACHMENT_OUTPUT<br/>present on present queue"]):::frame

    BASE --> RP --> FB --> PIPE --> SEM --> CB --> REC --> DRAW

    click BASE "./tutorialbase.md" "TutorialBase"
```

**Linked steps:** device and swapchain setup is
[TutorialBase](tutorialbase.md), which packages
[Tutorial01](01_device_initialization.md) and
[Tutorial02](02_swap_chain.md).

## What's new

- Render pass with `loadOp = CLEAR`; the clear no longer needs a transfer
  command.
- Framebuffers over the swapchain image views.
- Shader modules (`VulkanCommon::createShaderModule()`) and the first
  `VkPipeline`.
- Command pool moves from the present family to the graphics family.

## Spec links

[`vkCreateRenderPass`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateRenderPass.html) ·
[`vkCreateFramebuffer`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateFramebuffer.html) ·
[`vkCreateShaderModule`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateShaderModule.html) ·
[`vkCreatePipelineLayout`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreatePipelineLayout.html) ·
[`vkCreateGraphicsPipelines`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateGraphicsPipelines.html) ·
[`vkCmdBeginRenderPass`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdBeginRenderPass.html) ·
[`vkCmdDraw`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDraw.html)

Source: `bin/03_first_triangle/Tutorial03.cpp`

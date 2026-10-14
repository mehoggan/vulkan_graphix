# Tutorial04 — Vertex attributes

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial03](03_first_triangle.md) · Next:
[Tutorial05](05_staging_resources.md)

Draws a colored quad from a vertex buffer, and replaces Tutorial03's
pre-recorded command buffers with a per-frame loop over three frames in
flight. Tutorial05–10 keep this loop.

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef reused fill:#eceff1,stroke:#607d8b,stroke-dasharray:4 3,color:#263238
    classDef changed fill:#fff3e0,stroke:#ef6c00,color:#3e2723
    classDef new fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    BASE["↩ TutorialBase<br/>prepareVulkan()"]:::reused
    RP["<b>createRenderPass</b> (changed)<br/>same attachment as Tutorial03<br/>+ 2 subpass dependencies<br/>(EXTERNAL → 0, 0 → EXTERNAL)"]:::changed
    PIPE["<b>createPipeline</b> (changed)<br/>1 vertex binding: position vec4 + color vec4<br/>TRIANGLE_STRIP<br/>dynamic viewport + scissor"]:::changed
    VB["<b>createVertexBuffer</b><br/>HOST_VISIBLE memory (BufferFactory)<br/>vkMapMemory → memcpy<br/>→ vkFlushMappedMemoryRanges → unmap"]:::new
    RR["<b>createRenderingResources</b><br/>pool + 3 command buffers<br/>3 image-available semaphores · 3 fences<br/>1 rendering-finished semaphore per image"]:::new
    DRAW(["<b>draw()</b> — next of 3 frames<br/>wait fence · acquire<br/>prepareFrame: create framebuffer for this image,<br/>record: barrier · begin pass · set viewport/scissor<br/>vkCmdBindVertexBuffers · vkCmdDraw(4)<br/>submit · present"]):::frame

    BASE --> RP --> PIPE --> VB --> RR --> DRAW

    click BASE "./tutorialbase.md" "TutorialBase"
```

**Linked steps:** [TutorialBase](tutorialbase.md). The render pass and
pipeline are Tutorial03's ([page](03_first_triangle.md)) with the changes
above.

## What's new

- Vertex input state and a host-visible vertex buffer.
- Frames in flight: three sets of command buffer, semaphore, and fence,
  re-recorded every frame (`VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT`).
- The framebuffer is made at record time for the acquired image.
- Viewport and scissor are dynamic state, so a resize doesn't require a
  new pipeline for them.

## Spec links

[`VkVertexInputAttributeDescription`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkVertexInputAttributeDescription.html) ·
[`VkSubpassDependency`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkSubpassDependency.html) ·
[`vkMapMemory`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkMapMemory.html) ·
[`vkFlushMappedMemoryRanges`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkFlushMappedMemoryRanges.html) ·
[`vkCmdBindVertexBuffers`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdBindVertexBuffers.html) ·
[`vkCmdSetViewport`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdSetViewport.html)

Source: `bin/04_vertex_attributes/Tutorial04.cpp`

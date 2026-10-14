# Tutorial05 — Staging resources

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial04](04_vertex_attributes.md) · Next:
[Tutorial06](06_descriptor_sets.md)

Same quad and same shaders as Tutorial04, but the vertex buffer lives in
device-local memory and is filled through a staging buffer.

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef reused fill:#eceff1,stroke:#607d8b,stroke-dasharray:4 3,color:#263238
    classDef changed fill:#fff3e0,stroke:#ef6c00,color:#3e2723
    classDef new fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    BASE["↩ TutorialBase<br/>prepareVulkan()"]:::reused
    RR["↩ Tutorial04<br/>createRenderingResources<br/>3 frames in flight"]:::reused
    RP["↩ Tutorial04<br/>createRenderPass"]:::reused
    PIPE["↩ Tutorial04<br/>createPipeline<br/>shader.05 = same GLSL as shader.04"]:::reused
    VB["<b>createVertexBuffer</b> (changed)<br/>VERTEX_BUFFER | TRANSFER_DST<br/>DEVICE_LOCAL memory"]:::changed
    STG["<b>createStagingBuffer</b><br/>4000 bytes · TRANSFER_SRC<br/>HOST_VISIBLE"]:::new
    COPY["<b>copyVertexData</b><br/>StagedUploader::uploadToBuffer<br/>map + memcpy into staging<br/>vkCmdCopyBuffer staging → vertex<br/>barrier TRANSFER_WRITE → VERTEX_ATTRIBUTE_READ<br/>one-off submit + vkDeviceWaitIdle"]:::new
    DRAW(["↩ Tutorial04<br/>draw() / prepareFrame"]):::reused

    BASE --> RR --> RP --> PIPE --> VB --> STG --> COPY --> DRAW

    click BASE "./tutorialbase.md" "TutorialBase"
    click RR "./04_vertex_attributes.md" "Tutorial04"
    click RP "./04_vertex_attributes.md" "Tutorial04"
    click PIPE "./04_vertex_attributes.md" "Tutorial04"
    click DRAW "./04_vertex_attributes.md" "Tutorial04"
```

**Linked steps:** [TutorialBase](tutorialbase.md);
rendering resources, render pass, pipeline, and the draw loop are
[Tutorial04](04_vertex_attributes.md)'s.

## What's new

- Device-local vertex memory, which the CPU can't write directly.
- A staging buffer and a one-time transfer submit, borrowing the first
  frame's command buffer. This is the `StagedUploader` path every later
  device-local buffer and texture uses.

## Spec links

[`vkCmdCopyBuffer`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdCopyBuffer.html) ·
[`VkBufferMemoryBarrier`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkBufferMemoryBarrier.html) ·
[`VkMemoryPropertyFlagBits`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkMemoryPropertyFlagBits.html)

Source: `bin/05_staging_resources/Tutorial05.cpp`

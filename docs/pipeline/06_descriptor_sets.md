# Tutorial06 — Descriptor sets

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial05](05_staging_resources.md) · Next:
[Tutorial07](07_uniform_buffers.md)

Textures the quad. Adds an image, view, and sampler uploaded through the
staging buffer, and the descriptor set that hands them to the fragment
shader. The render pass stops doing the swapchain layout transitions, so
each frame records them as explicit barriers instead.

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef reused fill:#eceff1,stroke:#607d8b,stroke-dasharray:4 3,color:#263238
    classDef changed fill:#fff3e0,stroke:#ef6c00,color:#3e2723
    classDef new fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    BASE["↩ TutorialBase<br/>prepareVulkan()"]:::reused
    RR["↩ Tutorial04<br/>createRenderingResources"]:::reused
    STG["↩ Tutorial05<br/>createStagingBuffer<br/>now 1 MB, big enough for a texture"]:::reused
    TEX["<b>createTexture</b><br/>texture.06.png → R8G8B8A8_UNORM image<br/>TRANSFER_DST | SAMPLED · DEVICE_LOCAL<br/>image view · sampler CLAMP_TO_EDGE<br/>uploadToImage: UNDEFINED → TRANSFER_DST<br/>vkCmdCopyBufferToImage → SHADER_READ_ONLY"]:::new
    DSL["<b>createDescriptorSetLayout</b><br/>binding 0: COMBINED_IMAGE_SAMPLER, fragment"]:::new
    POOL["<b>createDescriptorPool</b> + <b>allocateDescriptorSet</b><br/>1 sampler descriptor · maxSets 1"]:::new
    UPD["<b>updateDescriptorSet</b><br/>vkUpdateDescriptorSets with the image info"]:::new
    RP["<b>createRenderPass</b> (changed)<br/>color stays COLOR_ATTACHMENT_OPTIMAL<br/>before and after · no dependencies"]:::changed
    PLL["<b>createPipelineLayout</b><br/>references the set layout"]:::new
    PIPE["<b>createPipeline</b> (changed)<br/>position vec4 + texcoord vec2<br/>TRIANGLE_STRIP · cull BACK · CCW"]:::changed
    VB["↩ Tutorial05<br/>createVertexBuffer<br/>device-local, via staging"]:::reused
    DRAW(["<b>draw() / prepareFrame</b> (changed)<br/>↩ Tutorial04's 3-frame loop, plus:<br/>barrier UNDEFINED → COLOR_ATTACHMENT before the pass<br/>vkCmdBindDescriptorSets<br/>barrier COLOR_ATTACHMENT → PRESENT_SRC after it"]):::frame

    BASE --> RR --> STG --> TEX --> DSL --> POOL --> UPD --> RP --> PLL --> PIPE --> VB --> DRAW
```

**Where the reused boxes come from:**

| Box | Defined in |
|---|---|
| prepareVulkan() | [TutorialBase](tutorialbase.md) |
| createRenderingResources | [Tutorial04](04_vertex_attributes.md) |
| createStagingBuffer | [Tutorial05](05_staging_resources.md) |
| createVertexBuffer | [Tutorial05](05_staging_resources.md) |

## What's new

- Images: device-local optimal-tiling image, view, sampler, and a staged
  upload with two layout transitions (`StagedUploader::uploadToImage`).
- Descriptor set layout, pool, set, and write. The pipeline layout is no
  longer empty.
- The swapchain image's layout changes move out of the render pass into
  per-frame barriers. `FrameLoop` keeps this design for Tutorial11–22.

## Spec links

[`vkCreateImage`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateImage.html) ·
[`vkCreateSampler`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateSampler.html) ·
[`vkCmdCopyBufferToImage`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdCopyBufferToImage.html) ·
[`VkImageMemoryBarrier`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkImageMemoryBarrier.html) ·
[`vkCreateDescriptorSetLayout`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateDescriptorSetLayout.html) ·
[`vkCreateDescriptorPool`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateDescriptorPool.html) ·
[`vkAllocateDescriptorSets`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkAllocateDescriptorSets.html) ·
[`vkUpdateDescriptorSets`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkUpdateDescriptorSets.html) ·
[`vkCmdBindDescriptorSets`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdBindDescriptorSets.html)

Source: `bin/06_descriptor_sets/Tutorial06.cpp`

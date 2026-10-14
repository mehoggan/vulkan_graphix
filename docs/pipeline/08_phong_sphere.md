# Tutorial08 — Phong sphere

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial07](07_uniform_buffers.md) · Next:
[Tutorial09](09_tessellated_terrain.md)

A lit, rotating icosphere. Goes 3D: depth buffer, index buffer,
perspective projection, and a uniform buffer rewritten every frame. There
is no texture in this one.

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef reused fill:#eceff1,stroke:#607d8b,stroke-dasharray:4 3,color:#263238
    classDef changed fill:#fff3e0,stroke:#ef6c00,color:#3e2723
    classDef new fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    BASE["↩ TutorialBase<br/>prepareVulkan()"]:::reused
    RR["↩ Tutorial04<br/>createRenderingResources"]:::reused
    STG["↩ Tutorial06<br/>createStagingBuffer"]:::reused
    DEPTH["<b>createDepthResources</b><br/>D32_SFLOAT image<br/>DEPTH_STENCIL_ATTACHMENT · DEVICE_LOCAL<br/>view with DEPTH aspect"]:::new
    UBO["<b>createUniformBuffer</b> (changed)<br/>HOST_VISIBLE | HOST_COHERENT, no staging<br/>model · view · projection<br/>+ light position, light color, view position"]:::changed
    DSL["<b>createDescriptorSetLayout</b> (changed)<br/>binding 0: UNIFORM_BUFFER<br/>vertex + fragment · no sampler"]:::changed
    POOL["↩ Tutorial07 shape<br/>createDescriptorPool + allocateDescriptorSet<br/>+ updateDescriptorSet<br/>now 1 uniform descriptor only"]:::reused
    RP["<b>createRenderPass</b> (changed)<br/>+ depth attachment: CLEAR<br/>UNDEFINED → DEPTH_STENCIL_ATTACHMENT_OPTIMAL"]:::changed
    PLL["↩ Tutorial06<br/>createPipelineLayout"]:::reused
    PIPE["<b>createPipeline</b> (changed)<br/>position vec4 + normal vec3<br/>TRIANGLE_LIST · cull BACK · <i>CW</i><br/>depth test + write, LESS<br/>Phong shaders"]:::changed
    VB["<b>createVertexBuffer</b> (changed)<br/>Math::Sphere icosphere<br/>device-local via staging"]:::changed
    IB["<b>createIndexBuffer</b><br/>INDEX_BUFFER | TRANSFER_DST<br/>device-local via staging"]:::new
    DRAW(["<b>draw()</b> (changed)<br/>vkDeviceWaitIdle → rewrite UBO (rotating model)<br/>↩ Tutorial06 frame recording, plus:<br/>framebuffer gets the depth view · clear depth 1.0<br/>vkCmdBindIndexBuffer · vkCmdDrawIndexed"]):::frame

    BASE --> RR --> STG --> DEPTH --> UBO --> DSL --> POOL --> RP --> PLL --> PIPE --> VB --> IB --> DRAW
```

**Where the reused boxes come from:**

| Box | Defined in |
|---|---|
| prepareVulkan() | [TutorialBase](tutorialbase.md) |
| createRenderingResources | [Tutorial04](04_vertex_attributes.md) |
| createStagingBuffer | [Tutorial06](06_descriptor_sets.md) |
| createDescriptorPool + allocateDescriptorSet | [Tutorial07](07_uniform_buffers.md) |
| createPipelineLayout | [Tutorial06](06_descriptor_sets.md) |

## What's new

- Depth image and a second render pass attachment.
- Index buffer and indexed draws.
- Perspective projection (`Tools::getPerspectiveProjectionMatrix`).
- One shared host-visible uniform buffer, rewritten each frame after
  `vkDeviceWaitIdle`. Tutorial09–22 all use this approach.
- Front face is clockwise to match the sphere's winding.

## Spec links

[`VkPipelineDepthStencilStateCreateInfo`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkPipelineDepthStencilStateCreateInfo.html) ·
[`vkCmdBindIndexBuffer`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdBindIndexBuffer.html) ·
[`vkCmdDrawIndexed`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDrawIndexed.html)

Source: `bin/08_phong_sphere/Tutorial08.cpp`

# Tutorial09 — Tessellated terrain

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial08](08_phong_sphere.md), with the texture path from
[Tutorial07](07_uniform_buffers.md) · Next:
[Tutorial10](10_spline_polyline.md)

A lit, textured, tessellated flat terrain viewed through a mouse-driven
orbit camera. Same 3D setup as Tutorial08, with the texture brought back
and backface culling turned off so the underside is visible.

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
    DEPTH["↩ Tutorial08<br/>createDepthResources (D32_SFLOAT)"]:::reused
    TEX["<b>createTexture</b> (changed)<br/>↩ Tutorial06/07 path<br/>texture.09.png · sampler <i>REPEAT</i><br/>UVs go past 1 so the ground tiles"]:::changed
    UBO["↩ Tutorial08<br/>createUniformBuffer<br/>host-visible, rewritten per frame"]:::reused
    DSL["<b>createDescriptorSetLayout</b> (changed)<br/>binding 0: UNIFORM_BUFFER, vertex + fragment<br/>binding 1: COMBINED_IMAGE_SAMPLER, fragment"]:::changed
    POOL["<b>createDescriptorPool</b> (changed)<br/>+ allocate + update<br/>1 uniform + 1 sampler · 2 writes"]:::changed
    RP["↩ Tutorial08<br/>createRenderPass (color + depth)"]:::reused
    PLL["↩ Tutorial06<br/>createPipelineLayout"]:::reused
    PIPE["<b>createPipeline</b> (changed)<br/>position + normal + texcoord<br/>cull <i>NONE</i> · depth test LESS<br/>fragment shader flips back-facing normals"]:::changed
    VB["<b>createVertexBuffer</b> (changed)<br/>two triangles over a square, subdivided by<br/>Math::tessellateTrianglesByMidpointSubdivision"]:::changed
    IB["↩ Tutorial08<br/>createIndexBuffer<br/>indices from the tessellator"]:::reused
    DRAW(["<b>draw()</b> (changed)<br/>↩ Tutorial08 loop · static identity model<br/>view from OrbitCamera every frame"]):::frame
    CAM["<b>OrbitCamera</b><br/>onMouseButton / onMouseMove<br/>left-drag orbits · wheel zooms"]:::new

    BASE --> RR --> STG --> DEPTH --> TEX --> UBO --> DSL --> POOL --> RP --> PLL --> PIPE --> VB --> IB --> DRAW
    CAM -. mouse .-> DRAW
```

**Where the reused boxes come from:**

| Box | Defined in |
|---|---|
| prepareVulkan() | [TutorialBase](tutorialbase.md) |
| createRenderingResources | [Tutorial04](04_vertex_attributes.md) |
| createStagingBuffer | [Tutorial06](06_descriptor_sets.md) |
| createDepthResources (D32_SFLOAT) | [Tutorial08](08_phong_sphere.md) |
| createTexture | [Tutorial07](07_uniform_buffers.md) |
| createUniformBuffer | [Tutorial08](08_phong_sphere.md) |
| createRenderPass (color + depth) | [Tutorial08](08_phong_sphere.md) |
| createPipelineLayout | [Tutorial06](06_descriptor_sets.md) |
| createIndexBuffer | [Tutorial08](08_phong_sphere.md) |

## What's new

- All three vertex attributes together: position, normal, texcoord.
- `VK_SAMPLER_ADDRESS_MODE_REPEAT` tiling.
- Culling off. Every pipeline before this one culled back faces.
- Mouse input and `OrbitCamera`, later moved to `lib/` and shared by
  every 3D tutorial.

Source: `bin/09_tessellated_terrain/Tutorial09.cpp`

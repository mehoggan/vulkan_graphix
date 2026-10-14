# Tutorial14 — Particle sphere

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial08](08_phong_sphere.md), on [VulkanCommon](vulkancommon.md) ·
Next: [Tutorial15](15_text_and_button.md)

A translucent, alpha-blended sphere, the look of vulkan_earth's particles
and shields. Same `Math::Sphere` and vertex shader as Tutorial08, with a
new fragment shader and blending turned on.

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef reused fill:#eceff1,stroke:#607d8b,stroke-dasharray:4 3,color:#263238
    classDef changed fill:#fff3e0,stroke:#ef6c00,color:#3e2723
    classDef new fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20
    classDef shared fill:#e3f2fd,stroke:#1565c0,color:#0d47a1
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    BASE["↩ TutorialBase<br/>prepareVulkan()"]:::reused
    RC["↩ VulkanCommon<br/>ResourceContext::initialize"]:::shared
    UBO["↩ Tutorial08<br/>createUniformBuffer<br/>matrices + light, rewritten per frame"]:::reused
    DESC["↩ Tutorial08<br/>binding 0: UBO (vert + frag)<br/>no texture"]:::reused
    RP["<b>createRenderPass</b><br/>color only, no depth"]:::shared
    PIPE["<b>createGraphicsPipeline</b> (changed)<br/>vertex shader = Tutorial08's · new fragment shader<br/>position + normal · cull BACK · CW<br/><i>alphaBlend()</i>: SRC_ALPHA / ONE_MINUS_SRC_ALPHA"]:::changed
    GEO["↩ Tutorial08<br/>Math::Sphere vertices + indices<br/>createDeviceLocalBuffer × 2"]:::reused
    FL["↩ VulkanCommon<br/>FrameLoop::create"]:::shared
    DRAW(["<b>draw()</b><br/>write UBO from OrbitCamera<br/>bind VB + IB + set · vkCmdDrawIndexed"]):::frame

    BASE --> RC --> UBO --> DESC --> RP --> PIPE --> GEO --> FL --> DRAW
```

**Where the reused boxes come from:**

| Box | Defined in |
|---|---|
| prepareVulkan() | [TutorialBase](tutorialbase.md) |
| ResourceContext::initialize | [VulkanCommon](vulkancommon.md) |
| createRenderPass | [VulkanCommon](vulkancommon.md) |
| FrameLoop::create | [VulkanCommon](vulkancommon.md) |
| createUniformBuffer | [Tutorial08](08_phong_sphere.md) |
| binding 0: UBO (vert + frag) | [Tutorial08](08_phong_sphere.md) |
| Math::Sphere vertices + indices | [Tutorial08](08_phong_sphere.md) |

## What's new

- Alpha blending (`VulkanCommon::alphaBlend()`), the first pipeline with
  blending enabled. [Tutorial20](20_particle_effects.md) builds on this
  sphere.

Source: `bin/14_particle_sphere/Tutorial14.cpp`

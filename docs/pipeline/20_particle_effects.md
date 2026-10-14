# Tutorial20 — Particle effects

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial14](14_particle_sphere.md) and
[Tutorial16](16_tank.md)'s per-draw push constants · Next:
[Tutorial21](21_world_scene.md)

vulkan_earth's real effects: three particle emitters (smoke, acid, float),
a growing and fading explosion, and the translucent shield. All of them
are one shared sphere mesh, drawn once per instance with its own model
matrix and color pushed as constants. The first tutorial with no texture
at all and a simulation that changes every frame.

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
    UBO["<b>createUniformBuffer</b> (changed)<br/>view + projection only<br/>binding 0: UBO, vertex stage"]:::changed
    RP["↩ Tutorial14<br/>createRenderPass (color only)"]:::reused
    PLL["<b>createPipelineLayout</b> (changed)<br/>push constant: mat4 model + vec4 color<br/>vertex + fragment stages<br/>↩ Tutorial18's two-stage range"]:::changed
    PIPE["<b>createGraphicsPipeline</b> (changed)<br/>new shader.20 · position only, unlit<br/>↩ Tutorial14: cull BACK · CW · alpha blend"]:::changed
    GEO["↩ Tutorial14<br/>Math::Sphere vertices + indices"]:::reused
    FL["↩ VulkanCommon<br/>FrameLoop::create"]:::shared
    SIM["<b>updateParticles / updateExplosion</b><br/>EffectSimulation: same code the game runs<br/>ParticleGenerator 1000-slot pools"]:::new
    DRAW(["<b>draw()</b><br/>step simulation · write UBO from OrbitCamera<br/>bind pipeline, VB, IB, set once<br/>draw_sphere(shield) · every live particle ·<br/>explosion: push model + color · vkCmdDrawIndexed"]):::frame

    BASE --> RC --> UBO --> RP --> PLL --> PIPE --> GEO --> FL --> DRAW
    SIM -. every frame .-> DRAW
```

**Where the reused boxes come from:**

| Box | Defined in |
|---|---|
| prepareVulkan() | [TutorialBase](tutorialbase.md) |
| ResourceContext::initialize | [VulkanCommon](vulkancommon.md) |
| FrameLoop::create | [VulkanCommon](vulkancommon.md) |
| createRenderPass (color only) | [Tutorial14](14_particle_sphere.md) |
| createPipelineLayout | [Tutorial18](18_player_tanks_hud.md) |
| createGraphicsPipeline | [Tutorial14](14_particle_sphere.md) |
| Math::Sphere vertices + indices | [Tutorial14](14_particle_sphere.md) |

Also builds on: [Tutorial16](16_tank.md).

## What's new

- Many instances of one mesh, each from a push constant. No per-instance
  buffers.
- CPU-side simulation (`EffectSimulation`) driving positions and colors
  every frame, independent of input.

Source: `bin/20_particle_effects/Tutorial20.cpp`

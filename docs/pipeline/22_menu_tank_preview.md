# Tutorial22 — Menu with a tank preview

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial15](15_text_and_button.md) (panel), [Tutorial16](16_tank.md)
(tank), and [Tutorial18](18_player_tanks_hud.md) (two pipelines) ·
Next: none (last tutorial)

vulkan_earth's `ReadyMenu` technique: a bevel-panel menu with a live,
spinning 3D tank rendered into a scissored sub-region of the screen.

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
    TEX["<b>Two textures</b><br/>tank: TestImage.raw (↩ Tutorial16)<br/>font atlas (↩ Tutorial15)"]:::reused
    UBO["<b>Two uniform buffers</b><br/>tank: view + projection (↩ Tutorial16)<br/>panel: orthographic (↩ Tutorial15)"]:::reused
    DESC["↩ Tutorial18<br/>two set layouts, pool of 2 sets<br/>tank set + panel set"]:::reused
    DEPTH["↩ Tutorial12<br/>createDepthImage · render pass color + depth"]:::reused
    PLL["↩ Tutorial18 shape<br/>tank layout: push constant mat4 model (vertex)<br/>panel layout: none"]:::reused
    PTANK["↩ Tutorial16<br/>tank pipeline: shader.22_3d = Tutorial16's GLSL<br/>depth test + write"]:::reused
    PPANEL["↩ Tutorial18 HUD<br/>panel pipeline: shader.22_panel = Tutorial15's GLSL<br/>depth off · alpha blend"]:::reused
    PARTS["↩ Tutorial16<br/>tank parts × 3"]:::reused
    PVB["↩ Tutorial15<br/>host-visible panel vertex buffer"]:::reused
    FL["↩ VulkanCommon<br/>FrameLoop::create(depth view)"]:::shared
    DRAW(["<b>draw()</b><br/>tank_angle += step · rebuild panel quads<br/>1 panel pipeline, full extent · vkCmdDraw<br/>2 tank pipeline: <i>vkCmdSetViewport + vkCmdSetScissor<br/>to the preview sub-rectangle</i><br/>per part push model (with spin) · vkCmdDraw"]):::frame

    BASE --> RC --> TEX --> UBO --> DESC --> DEPTH --> PLL --> PTANK --> PPANEL --> PARTS --> PVB --> FL --> DRAW
```

**Where the reused boxes come from:**

| Box | Defined in |
|---|---|
| prepareVulkan() | [TutorialBase](tutorialbase.md) |
| ResourceContext::initialize | [VulkanCommon](vulkancommon.md) |
| FrameLoop::create(depth view) | [VulkanCommon](vulkancommon.md) |
| Two textures | [Tutorial16](16_tank.md) |
| Two uniform buffers | [Tutorial16](16_tank.md) |
| two set layouts, pool of 2 sets | [Tutorial18](18_player_tanks_hud.md) |
| createDepthImage · render pass color + depth | [Tutorial12](12_generated_terrain.md) |
| tank layout: push constant mat4 model (vertex) | [Tutorial18](18_player_tanks_hud.md) |
| tank pipeline: shader.22_3d = Tutorial16's GLSL | [Tutorial16](16_tank.md) |
| panel pipeline: shader.22_panel = Tutorial15's GLSL | [Tutorial18](18_player_tanks_hud.md) |
| tank parts × 3 | [Tutorial16](16_tank.md) |
| host-visible panel vertex buffer | [Tutorial15](15_text_and_button.md) |

## What's new

Almost every setup box is grey. The one new technique is in `draw()`:

- Setting the dynamic viewport and scissor to a sub-rectangle before the
  tank's draws, so the 3D pass renders into part of the frame. Every
  earlier draw used the full-extent viewport `FrameLoop::begin()` sets.
- A transform that animates on its own (`tank_angle` grows every frame),
  sent through the push constant rather than the uniform buffer.

Source: `bin/22_menu_tank_preview/Tutorial22.cpp`

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

    click BASE "./tutorialbase.md" "TutorialBase"
    click RC "./vulkancommon.md" "VulkanCommon"
    click FL "./vulkancommon.md" "VulkanCommon"
    click TEX "./16_tank.md" "Tutorial16"
    click UBO "./16_tank.md" "Tutorial16"
    click DESC "./18_player_tanks_hud.md" "Tutorial18"
    click DEPTH "./12_generated_terrain.md" "Tutorial12"
    click PLL "./18_player_tanks_hud.md" "Tutorial18"
    click PTANK "./16_tank.md" "Tutorial16"
    click PPANEL "./18_player_tanks_hud.md" "Tutorial18"
    click PARTS "./16_tank.md" "Tutorial16"
    click PVB "./15_text_and_button.md" "Tutorial15"
```

**Linked steps:** [TutorialBase](tutorialbase.md);
[VulkanCommon](vulkancommon.md); [Tutorial12](12_generated_terrain.md)
(depth); [Tutorial15](15_text_and_button.md) (font, panel shaders,
per-frame vertex buffer); [Tutorial16](16_tank.md) (tank texture,
pipeline, parts); [Tutorial18](18_player_tanks_hud.md) (two layouts, two
pipelines, depth-off overlay).

## What's new

Almost every setup box is grey. The one new technique is in `draw()`:

- Setting the dynamic viewport and scissor to a sub-rectangle before the
  tank's draws, so the 3D pass renders into part of the frame. Every
  earlier draw used the full-extent viewport `FrameLoop::begin()` sets.
- A transform that animates on its own (`tank_angle` grows every frame),
  sent through the push constant rather than the uniform buffer.

Source: `bin/22_menu_tank_preview/Tutorial22.cpp`

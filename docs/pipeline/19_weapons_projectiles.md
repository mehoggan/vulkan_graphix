# Tutorial19 — Weapons and projectiles

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial16](16_tank.md) (3D), [Tutorial17](17_item_inventory.md)
(grid), and [Tutorial18](18_player_tanks_hud.md) (two pipelines) · Next:
[Tutorial20](20_particle_effects.md)

vulkan_earth's three projectile meshes (Default, Acid, BFB), each with its
own texture and scale, above a 5×2 weapon grid that mirrors Tutorial17's
item grid.

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
    PROJ["<b>Per projectile type</b> × 3<br/>loadRawTexture (512²)<br/>+ loadOglTexturedMesh → device-local VB<br/>↩ Tutorial13's loading"]:::changed
    GRIDTEX["↩ Tutorial17<br/>font atlas + icon atlas textures"]:::reused
    UBO["<b>Two uniform buffers</b><br/>3D: view + projection (↩ Tutorial16)<br/>grid: orthographic (↩ Tutorial17)"]:::changed
    DESC["<b>Two set layouts, pool of 3 + 2 sets</b><br/>3 sets for 3D, one per projectile texture<br/>font + icon sets ↩ Tutorial17"]:::changed
    RP["<b>createRenderPass</b><br/>color only, no depth"]:::shared
    PLL["<b>Two pipeline layouts</b><br/>3D: push constant mat4 model (↩ Tutorial16)<br/>grid: none"]:::reused
    P3D["↩ Tutorial16<br/>3D pipeline: shader.19_3d = Tutorial16's GLSL"]:::reused
    PGRID["↩ Tutorial17<br/>grid pipeline: shader.19_grid = Tutorial17's GLSL<br/>alpha blend"]:::reused
    VB["↩ Tutorial17<br/>host-visible grid vertex buffer"]:::reused
    FL["↩ VulkanCommon<br/>FrameLoop::create"]:::shared
    DRAW(["<b>draw()</b><br/>write 3D UBO · rebuild grid quads<br/>3D: per projectile: bind its set + VB<br/>push scaled model · vkCmdDraw<br/>grid: ↩ Tutorial17's font pass + icon pass"]):::frame

    BASE --> RC --> PROJ --> GRIDTEX --> UBO --> DESC --> RP --> PLL --> P3D --> PGRID --> VB --> FL --> DRAW
```

**Where the reused boxes come from:**

| Box | Defined in |
|---|---|
| prepareVulkan() | [TutorialBase](tutorialbase.md) |
| ResourceContext::initialize | [VulkanCommon](vulkancommon.md) |
| createRenderPass | [VulkanCommon](vulkancommon.md) |
| FrameLoop::create | [VulkanCommon](vulkancommon.md) |
| Per projectile type × 3 | [Tutorial13](13_projectile_model.md) |
| font atlas + icon atlas textures | [Tutorial17](17_item_inventory.md) |
| Two pipeline layouts | [Tutorial16](16_tank.md) |
| 3D pipeline: shader.19_3d = Tutorial16's GLSL | [Tutorial16](16_tank.md) |
| grid pipeline: shader.19_grid = Tutorial17's GLSL | [Tutorial17](17_item_inventory.md) |
| host-visible grid vertex buffer | [Tutorial17](17_item_inventory.md) |

Also builds on: [Tutorial18](18_player_tanks_hud.md).

## What's new

- N descriptor sets from one layout, one per texture, rebound per draw.
  Tutorial17 used two; this uses three for the 3D pass alone.
- Two pipelines in one pass with **no** depth buffer: the three meshes
  never overlap, so none is needed.
- Weapon data and projectile scale from `GameCatalog`.

Source: `bin/19_weapons_projectiles/Tutorial19.cpp`

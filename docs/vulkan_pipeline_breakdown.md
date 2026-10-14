# Vulkan pipeline setup, tutorial by tutorial

Each tutorial has its own page under [`pipeline/`](pipeline/) with one
Mermaid diagram of its setup, in the order the code runs it, followed by
its per-frame `draw()`. A step a tutorial takes unchanged from an earlier
one is a grey dashed box naming that tutorial and linking to its page,
so each page shows in color only what that tutorial adds or changes.

## Legend

```mermaid
flowchart LR
    classDef reused fill:#eceff1,stroke:#607d8b,stroke-dasharray:4 3,color:#263238
    classDef changed fill:#fff3e0,stroke:#ef6c00,color:#3e2723
    classDef new fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20
    classDef shared fill:#e3f2fd,stroke:#1565c0,color:#0d47a1
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    R["↩ TutorialNN<br/>same step as an earlier tutorial<br/>(links to it)"]:::reused
    C["changed from the<br/>tutorial it builds on"]:::changed
    N["new in this tutorial"]:::new
    S["shared library code<br/>(TutorialBase, VulkanCommon)"]:::shared
    F(["per-frame draw()"]):::frame
```

Boxes that point at another page carry a Mermaid `click` link. Not every
Markdown viewer follows those, so every page also lists the same links as
plain Markdown under its diagram.

## How the tutorials build on each other

Solid arrows: the tutorial is a modified copy of the one it points from.
Dotted arrows: it borrows one technique or shader from it.

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef shared fill:#e3f2fd,stroke:#1565c0,color:#0d47a1
    classDef tut fill:#ffffff,stroke:#455a64,color:#263238

    T01["01 Device init"]:::tut --> T02["02 Swapchain"]:::tut
    T02 --> TB["TutorialBase"]:::shared

    subgraph HAND ["Tutorial03–10: every Vulkan object written out"]
        T03["03 First triangle"]:::tut --> T04["04 Vertex attributes"]:::tut
        T04 --> T05["05 Staging"]:::tut --> T06["06 Descriptor sets"]:::tut
        T06 --> T07["07 Uniform buffers"]:::tut --> T08["08 Phong sphere"]:::tut
        T08 --> T09["09 Tessellated terrain"]:::tut --> T10["10 Spline polyline"]:::tut
        T07 -.-> T09
    end
    TB --> T03

    TB --> VC["VulkanCommon<br/>ResourceContext + FrameLoop"]:::shared

    subgraph LIB ["Tutorial11–22: built on VulkanCommon"]
        T11["11 Skybox"]:::tut
        T12["12 Generated terrain"]:::tut
        T13["13 Projectile model"]:::tut
        T14["14 Particle sphere"]:::tut
        T15["15 Text + button"]:::tut
        T16["16 Tank"]:::tut
        T17["17 Item inventory"]:::tut
        T18["18 Tanks + HUD"]:::tut
        T19["19 Weapons + projectiles"]:::tut
        T20["20 Particle effects"]:::tut
        T21["21 World scene"]:::tut
        T22["22 Menu tank preview"]:::tut

        T11 --> T13 --> T16
        T15 --> T17
        T16 --> T18
        T15 --> T18
        T16 --> T19
        T17 --> T19
        T14 --> T20
        T11 --> T21
        T12 --> T21
        T16 --> T21
        T16 --> T22
        T15 --> T22
        T18 -.-> T22
        T18 -.-> T19
        T14 -.-> T15
    end
    VC --> LIB

    T07 -.-> T11
    T09 -.-> T12
    T08 -.-> T14
    T10 -.-> T16

    click T01 "./pipeline/01_device_initialization.md"
    click T02 "./pipeline/02_swap_chain.md"
    click TB "./pipeline/tutorialbase.md"
    click T03 "./pipeline/03_first_triangle.md"
    click T04 "./pipeline/04_vertex_attributes.md"
    click T05 "./pipeline/05_staging_resources.md"
    click T06 "./pipeline/06_descriptor_sets.md"
    click T07 "./pipeline/07_uniform_buffers.md"
    click T08 "./pipeline/08_phong_sphere.md"
    click T09 "./pipeline/09_tessellated_terrain.md"
    click T10 "./pipeline/10_spline_polyline.md"
    click VC "./pipeline/vulkancommon.md"
    click T11 "./pipeline/11_skybox.md"
    click T12 "./pipeline/12_generated_terrain.md"
    click T13 "./pipeline/13_projectile_model.md"
    click T14 "./pipeline/14_particle_sphere.md"
    click T15 "./pipeline/15_text_and_button.md"
    click T16 "./pipeline/16_tank.md"
    click T17 "./pipeline/17_item_inventory.md"
    click T18 "./pipeline/18_player_tanks_hud.md"
    click T19 "./pipeline/19_weapons_projectiles.md"
    click T20 "./pipeline/20_particle_effects.md"
    click T21 "./pipeline/21_world_scene.md"
    click T22 "./pipeline/22_menu_tank_preview.md"
```

## Pages

| Page | Builds on | What it adds |
|---|---|---|
| [TutorialBase](pipeline/tutorialbase.md) | 01, 02 | Shared device + swapchain bring-up, image views, resize protocol |
| [01 Device initialization](pipeline/01_device_initialization.md) | — | Loader, instance, logical device, one queue |
| [02 Swap chain](pipeline/02_swap_chain.md) | 01 | Surface, present queue, swapchain, clear via `vkCmdClearColorImage` |
| [03 First triangle](pipeline/03_first_triangle.md) | TutorialBase | Render pass, framebuffers, shaders, first pipeline |
| [04 Vertex attributes](pipeline/04_vertex_attributes.md) | 03 | Host-visible vertex buffer, 3 frames in flight, dynamic viewport |
| [05 Staging resources](pipeline/05_staging_resources.md) | 04 | Device-local vertex buffer through a staging copy |
| [06 Descriptor sets](pipeline/06_descriptor_sets.md) | 05 | Texture + sampler, descriptor sets, per-frame layout barriers |
| [07 Uniform buffers](pipeline/07_uniform_buffers.md) | 06 | Uniform buffer, orthographic projection |
| [08 Phong sphere](pipeline/08_phong_sphere.md) | 07 | Depth buffer, index buffer, per-frame host-visible uniforms |
| [09 Tessellated terrain](pipeline/09_tessellated_terrain.md) | 08 (+07) | Texture with REPEAT, cull NONE, orbit camera |
| [10 Spline polyline](pipeline/10_spline_polyline.md) | 09 | Line strips, push constants, dynamic line width |
| [VulkanCommon](pipeline/vulkancommon.md) | 05–08 techniques | `ResourceContext` setup, `FrameLoop` per-frame loop |
| [11 Skybox](pipeline/11_skybox.md) | 07 shaders | First library-built tutorial; indexed textured cube |
| [12 Generated terrain](pipeline/12_generated_terrain.md) | 09 shaders | `TerrainGenerator`, depth through `FrameLoop` |
| [13 Projectile model](pipeline/13_projectile_model.md) | 11 | `.ogl` mesh loading |
| [14 Particle sphere](pipeline/14_particle_sphere.md) | 08 | Alpha blending |
| [15 Text and button](pipeline/15_text_and_button.md) | 07, 14 | Font atlas, per-frame host-visible UI vertices |
| [16 Tank](pipeline/16_tank.md) | 13, 10 | Multi-part mesh, model matrix as push constant |
| [17 Item inventory](pipeline/17_item_inventory.md) | 15 | Two descriptor sets from one layout, icon atlas |
| [18 Player tanks + HUD](pipeline/18_player_tanks_hud.md) | 16, 15 | Two pipelines in one pass, depth-off overlay |
| [19 Weapons + projectiles](pipeline/19_weapons_projectiles.md) | 16, 17, 18 | N descriptor sets, one per texture |
| [20 Particle effects](pipeline/20_particle_effects.md) | 14, 16 | Instancing by push constant, live simulation |
| [21 World scene](pipeline/21_world_scene.md) | 11, 12, 16 | Three pipelines, depth-off skybox, height-based placement |
| [22 Menu tank preview](pipeline/22_menu_tank_preview.md) | 15, 16, 18 | Sub-region viewport and scissor |

"Same GLSL" claims on these pages were checked by diffing the shader
sources under `resources/NN/Data/`, ignoring comments and whitespace.

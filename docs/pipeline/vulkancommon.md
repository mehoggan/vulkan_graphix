# Shared: `VulkanCommon` renderer scaffolding

[Index](../vulkan_pipeline_breakdown.md) · Used by Tutorial11–22 (and
`Render::Renderer`)

Tutorial03–10 write every Vulkan object out by hand. Tutorial11–22 build
the same objects through `include/vulkan_graphix/VulkanCommon/`, so each of
those pages shows only what is specific to that tutorial. This page is
what the boxes marked **ResourceContext** and **FrameLoop** stand for.

## Setup: `createResources()`

Every Tutorial11–22 `createResources()` follows this order. Each tutorial
page fills in its own textures, bindings, pipelines, and buffers.

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef reused fill:#eceff1,stroke:#607d8b,stroke-dasharray:4 3,color:#263238
    classDef shared fill:#e3f2fd,stroke:#1565c0,color:#0d47a1
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    BASE["↩ TutorialBase<br/>prepareVulkan()"]:::reused
    INIT["<b>ResourceContext::initialize</b><br/>binds device + graphics queue<br/>own upload command pool + buffer"]:::shared
    TEX["<b>Textures</b><br/>loadTexture (.png/.jpg) · loadRawTexture (.raw RGB)<br/>createTexture (RGBA pixels, e.g. a font atlas)<br/>image + view + sampler, staged upload<br/>↩ technique from Tutorial06"]:::shared
    UBO["<b>Uniform buffers</b><br/>createUniformBuffer: host-visible + coherent<br/>writeBuffer() each frame<br/>↩ technique from Tutorial08"]:::shared
    DESC["<b>Descriptors</b><br/>createDescriptorSetLayout(bindings)<br/>createDescriptorPool(bindings, N sets)<br/>allocateDescriptorSet · write*Descriptor<br/>↩ technique from Tutorial06/07"]:::shared
    DEPTH["<b>createDepthImage</b> (optional)<br/>D32_SFLOAT image + view<br/>↩ technique from Tutorial08"]:::shared
    RP["<b>createRenderPass(RenderPassDescription)</b><br/>color: CLEAR/STORE, stays COLOR_ATTACHMENT_OPTIMAL<br/>+ depth: CLEAR/DONT_CARE when m_depth_format set"]:::shared
    PL["<b>createPipelineLayout</b><br/>set layouts + push-constant ranges"]:::shared
    GP["<b>createGraphicsPipeline(GraphicsPipelineDescription)</b><br/>vert + frag SPIR-V · one vertex binding<br/>defaults: TRIANGLE_LIST, cull NONE, CCW,<br/>no depth state, opaque blend,<br/>dynamic viewport + scissor"]:::shared
    VB["<b>Geometry buffers</b><br/>createDeviceLocalBuffer: staged upload<br/>createHostVisibleBuffer: rewritten per frame<br/>↩ technique from Tutorial05"]:::shared
    FL["<b>FrameLoop::create</b><br/>3 frames in flight: cmd buffer, semaphore, fence<br/>per swapchain image: framebuffer<br/>(+ depth view) + rendering-finished semaphore"]:::shared

    BASE --> INIT --> TEX --> UBO --> DESC --> DEPTH --> RP --> PL --> GP --> VB --> FL

    click BASE "./tutorialbase.md" "TutorialBase"
```

**Linked steps:** [TutorialBase](tutorialbase.md). The techniques each
helper wraps were introduced in
[Tutorial05](05_staging_resources.md) (staging),
[Tutorial06](06_descriptor_sets.md) (textures, descriptors),
[Tutorial07](07_uniform_buffers.md) (uniform binding), and
[Tutorial08](08_phong_sphere.md) (depth, host-visible uniforms).

## Per frame: `draw()` through `FrameLoop::draw()`

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef shared fill:#e3f2fd,stroke:#1565c0,color:#0d47a1
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    CPU["<b>tutorial draw()</b><br/>vkDeviceWaitIdle<br/>writeBuffer(uniforms / per-frame vertices)"]:::frame
    BEGIN["<b>FrameLoop::begin</b><br/>vkWaitForFences · vkAcquireNextImageKHR<br/>vkBeginCommandBuffer<br/>barrier UNDEFINED → COLOR_ATTACHMENT_OPTIMAL<br/>vkCmdBeginRenderPass(clear values)<br/>full-extent viewport + scissor"]:::shared
    REC["<b>record(command_buffer)</b><br/>the tutorial's own binds,<br/>push constants, and draws"]:::frame
    END["<b>FrameLoop::end</b><br/>vkCmdEndRenderPass<br/>barrier → PRESENT_SRC_KHR<br/>(queue-family transfer if queues differ)<br/>vkQueueSubmit · vkQueuePresentKHR"]:::shared
    OOD["out of date / suboptimal<br/>→ base.onWindowSizeChanged()<br/>→ childClear(): frames.destroy() + releaseAll()<br/>→ createResources() again"]:::shared

    CPU --> BEGIN --> REC --> END
    BEGIN -.-> OOD
    END -.-> OOD
```

## Teardown

`childClear()` is two lines in every Tutorial11–22:
`m_frames.destroy(); m_resources.releaseAll();`. `releaseAll()` waits for
the device to go idle, then destroys everything the context created,
newest first.

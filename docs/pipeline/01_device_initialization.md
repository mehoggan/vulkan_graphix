# Tutorial01 — Device initialization

[Index](../vulkan_pipeline_breakdown.md) · Builds on: nothing ·
Next: [Tutorial02](02_swap_chain.md)

Loads Vulkan, creates an instance and a logical device, and fetches one
queue. No window surface, no swapchain, nothing drawn. `Tutorial01`
derives from `os::ProjectBase` directly; [`TutorialBase`](tutorialbase.md)
later absorbs these steps.

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef new fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    LIB["<b>loadVulkanLibrary</b><br/>dlopen libvulkan.so.1"]:::new
    EXP["<b>loadExportedEntryPoints</b><br/>dlsym → vkGetInstanceProcAddr"]:::new
    GLB["<b>loadGlobalLevelEntryPoints</b><br/>vkGetInstanceProcAddr(nullptr, …)<br/>vkCreateInstance · vkEnumerateInstance*"]:::new
    INST["<b>createInstance</b><br/>checkValidationLayerSupport<br/>apiVersion 1.3 · VK_EXT_debug_utils in debug<br/>vkCreateInstance · setupDebugMessenger"]:::new
    ILEP["<b>loadInstanceLevelEntryPoints</b><br/>vkGetInstanceProcAddr(instance, …)"]:::new
    DEV["<b>createDevice</b><br/>vkEnumeratePhysicalDevices<br/>checkPhysicalDeviceProperties:<br/>first family with VK_QUEUE_GRAPHICS_BIT<br/>1 queue · no device extensions"]:::new
    DLEP["<b>loadDeviceLevelEntryPoints</b><br/>vkGetDeviceProcAddr"]:::new
    Q["<b>getDeviceQueue</b><br/>one graphics queue"]:::new
    DONE(["Done — draw() does nothing"]):::frame

    LIB --> EXP --> GLB --> INST --> ILEP --> DEV --> DLEP --> Q --> DONE
```

## What's new

Everything; this is the starting point. The function-pointer loading
pattern (`ListOfFunctions.inl` + `VulkanFunctions.h`) is what the
fault-injection tests later swap out.

## Spec links

[`vkGetInstanceProcAddr`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetInstanceProcAddr.html) ·
[`vkCreateInstance`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateInstance.html) ·
[`vkCreateDebugUtilsMessengerEXT`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateDebugUtilsMessengerEXT.html) ·
[`vkEnumeratePhysicalDevices`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkEnumeratePhysicalDevices.html) ·
[`vkGetPhysicalDeviceQueueFamilyProperties`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceQueueFamilyProperties.html) ·
[`vkCreateDevice`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateDevice.html) ·
[`vkGetDeviceProcAddr`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetDeviceProcAddr.html) ·
[`vkGetDeviceQueue`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetDeviceQueue.html)

Source: `bin/01_device_initialization/Tutorial01.cpp`

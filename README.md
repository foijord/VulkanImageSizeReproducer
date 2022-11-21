## Unable to create VkImage of maximum extent ##
When creating a VkImage of the maximum extent reported by vkGetPhysicalDeviceImageFormatProperties, vkCreateImage returns VK_ERROR_OUT_OF_DEVICE_MEMORY.

## Build ##
### Prerequisites ###
- Windows 10
- Microsoft Visual Studio 2022 (64-bit) Version 17.3.2 or later with the "Desktop development with C++" Workload installed.
- NVIDIA GPU, Pascal or more recent
- NVIDIA graphics driver installed (version 526.67)
- Vulkan SDK installed (1.3.231)

Open a Developer PowerShell for Visual Studio 2022. Create a Build folder inside this repo, build and execute:

```
PS C:\code\VulkanImageSizeReproducer\Build> cmake.exe ..
PS C:\code\VulkanImageSizeReproducer\Build> MSBuild.exe .\VulkanImageSizeReproducer.sln
PS C:\code\VulkanImageSizeReproducer\Build> .\Debug\VulkanImageSizeReproducer.exe
```

Output on my system:
```
Creating device: Quadro RTX 8000
NVIDIA Driver version: 526.67.0.0
VkImageFormatPropeties.maxExtent = { 16384, 16384, 16384 }
VkPhysicalDeviceFormatProperties2.properties.limits.maxImageDimension3D = 16384
Creating image with extent: { 16384, 16384, 16384 }
vkCreateImage returned VK_ERROR_OUT_OF_DEVICE_MEMORY
```

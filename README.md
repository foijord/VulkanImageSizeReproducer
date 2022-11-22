## Unable to create VkImage of maximum extent ##
When creating a VkImage of the maximum extent reported by vkGetPhysicalDeviceImageFormatProperties, vkCreateImage returns VK_ERROR_OUT_OF_DEVICE_MEMORY on NVIDIA GPU, but succeeds on Intel.

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
PS C:\code\VulkanImageSizeReproducer\Build> .\Debug\VulkanImageMaxSizeReproducer.exe
```

Output from Laptop:
```
Creating device: NVIDIA RTX A5000 Laptop GPU
NVIDIA Driver version: 517.40.0.0
VkImageFormatPropeties.maxExtent = { 16384, 16384, 16384 }
VkPhysicalDeviceFormatProperties2.properties.limits.maxImageDimension3D = 16384
Creating image with extent: { 16384, 16384, 16384 }
vkCreateImage returned VK_ERROR_OUT_OF_DEVICE_MEMORY

Creating device: Intel(R) UHD Graphics
Intel Driver version: 100.533
VkImageFormatPropeties.maxExtent = { 16384, 16384, 2048 }
VkPhysicalDeviceFormatProperties2.properties.limits.maxImageDimension3D = 2048
Creating image with extent: { 16384, 16384, 2048 }
vkCreateImage returned VK_SUCCESS
```
Output from Desktop:
```
Creating device: Quadro RTX 8000
NVIDIA Driver version: 526.67.0.0
VkImageFormatPropeties.maxExtent = { 16384, 16384, 16384 }
VkPhysicalDeviceFormatProperties2.properties.limits.maxImageDimension3D = 16384
Creating image with extent: { 16384, 16384, 16384 }
vkCreateImage returned VK_ERROR_OUT_OF_DEVICE_MEMORY
```

## Creating and deleting many large VkImages eventually fails ##
When creating a VkImage of of extent 1024 x 1024 x 1024 and deleting it, vkCreateImage returns VK_ERROR_OUT_OF_DEVICE_MEMORY after many iterations.

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
PS C:\code\VulkanImageSizeReproducer\Build> .\Debug\VulkanImageMemoryLeakReproducer.exe
```

Output from Desktop:
```
Creating device: Quadro RTX 8000
NVIDIA Driver version: 526.67.0.0
Creating image with extent: { 1024, 1024, 1024 }
vkCreateImage returned VK_ERROR_OUT_OF_DEVICE_MEMORY after 883 iterations
```
# This repo contains code that can be used to reproduce two issues with vkCreateImage on NVIDIA GPUs #

## First issue: Unable to create VkImage of maximum extent ##
```
VkFormat format = VK_FORMAT_R8_UNORM;
VkImageType imageType = VK_IMAGE_TYPE_3D;
VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
VkImageCreateFlags flags = VK_IMAGE_CREATE_SPARSE_BINDING_BIT | VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT;

VkImageFormatProperties imageFormatProperties;
vkGetPhysicalDeviceImageFormatProperties(physicalDevice, format, imageType, tiling, usage, flags, &imageFormatProperties);

VkImageCreateInfo imageCreateInfo{
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .pNext = nullptr,
    .flags = flags,
    .imageType = imageType,
    .format = format,
    .extent = imageFormatProperties.maxExtent,
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .tiling = tiling,
    .usage = usage,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = nullptr,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
};

VkImage image;
auto result = vkCreateImage(device, &imageCreateInfo, nullptr, &image);
```
This code snippet queries vkGetPhysicalDeviceImageFormatProperties for the physical device and uses the returned maxExtent as the extent in VkImageCreateInfo. Expected result is that this succeeds. (Largest extent that succeeds on NVIDIA is { 16384, 16384, 3520 })

Output from reproducer:
```
Creating device: NVIDIA RTX A5000 Laptop GPU
NVIDIA Driver version: driver version 517.40.0.0
VkImageFormatProperties.maxExtent = { 16384, 16384, 16384 }
VkImageFormatProperties.maxResourceSize = 18446744073709551615
VkPhysicalDeviceProperties2.properties.limits.maxImageDimension3D = 16384
Creating image with extent = { 16384, 16384, 16384 }
vkCreateImage returned VK_ERROR_OUT_OF_DEVICE_MEMORY

Creating device: Quadro RTX 8000
NVIDIA Driver version: 526.67.0.0
VkImageFormatPropeties.maxExtent = { 16384, 16384, 16384 }
VkIimageFormatProperties.maxResourceSize = 18446744073709551615
VkPhysicalDeviceProperties2.properties.limits.maxImageDimension3D = 16384
Creating image with extent = { 16384, 16384, 16384 }
vkCreateImage returned VK_ERROR_OUT_OF_DEVICE_MEMORY

Creating device: Intel(R) UHD Graphics
Intel Driver version: 100.533
VkImageFormatProperties.maxExtent = { 16384, 16384, 2048 }
VkImageFormatProperties.maxResourceSize = 824634769408
Creating image with extent = { 16384, 16384, 2048 }
VkPhysicalDeviceProperties2.properties.limits.maxImageDimension3D = 2048
vkCreateImage returned VK_SUCCESS
```
## Second issue: repeatedly creating and deleting VkImages eventually fails ##
```
VkImageCreateInfo imageCreateInfo{
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .pNext = nullptr,
    .flags = flags,
    .imageType = imageType,
    .format = format,
    .extent = { 1024, 1024, 1024 },
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .tiling = tiling,
    .usage = usage,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = nullptr,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
};

VkResult result;
for (size_t count = 0; count < 1000; count++) {
    VkImage image;
    result = vkCreateImage(device, &imageCreateInfo, nullptr, &image);
    if (result == VK_SUCCESS) {
        vkDestroyImage(device, image, nullptr);
    }
    else {
        if (result == VK_ERROR_OUT_OF_DEVICE_MEMORY) {
            std::cerr << "vkCreateImage returned VK_ERROR_OUT_OF_DEVICE_MEMORY" << " after " << count << " iterations" << std::endl;
        }
        break;
    }
}

```
When repeatedly creating and deleting a VkImage of of extent { 1024, 1024, 1024 }, vkCreateImage returns VK_ERROR_OUT_OF_DEVICE_MEMORY after many iterations. 

```
Creating device: NVIDIA RTX A5000 Laptop GPU
NVIDIA Driver version: 517.40.0.0
Creating image with extent: { 1024, 1024, 1024 }
vkCreateImage returned VK_ERROR_OUT_OF_DEVICE_MEMORY after 883 iterations

Creating device: Intel(R) UHD Graphics
Intel Driver version: 100.533
Creating image with extent: { 1024, 1024, 1024 }
Created and deleted 1000 images.

Creating device: Quadro RTX 8000
NVIDIA Driver version: 526.67.0.0
Creating image with extent: { 1024, 1024, 1024 }
vkCreateImage returned VK_ERROR_OUT_OF_DEVICE_MEMORY after 883 iterations
```

## Build and Reproduce ##
### Prerequisites ###
- Windows 10
- Microsoft Visual Studio 2022 (64-bit) Version 17.3.2 or later with the "Desktop development with C++" Workload installed.
- NVIDIA GPU, Pascal or more recent
- NVIDIA graphics driver installed (version 526.67)
- Vulkan SDK installed (1.3.231, with Volk)

Open a Developer PowerShell for Visual Studio 2022. Create a Build folder inside this repo, build and execute:

```
PS C:\code\VulkanImageSizeReproducer\Build> cmake.exe ..
PS C:\code\VulkanImageSizeReproducer\Build> MSBuild.exe .\VulkanImageSizeReproducer.sln
```

Execute max image size creation:
```
PS C:\code\VulkanImageSizeReproducer\Build> .\Debug\VulkanImageMaxSizeReproducer.exe
```

Execute repeatedly creating and deleting VkImage:
```
PS C:\code\VulkanImageSizeReproducer\Build> .\Debug\VulkanImageMemoryLeakReproducer.exe
```

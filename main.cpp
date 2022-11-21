
#pragma once

#define VOLK_IMPLEMENTATION
#ifdef _WIN32
#include <Volk/volk.h>
#else
#include <volk.h>
#endif

#include <map>
#include <string>
#include <vector>
#include <iostream>


int main(int, char* [])
{
	volkInitialize();

	VkApplicationInfo applicationInfo{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = "Image Size Test",
		.applicationVersion = 1,
		.pEngineName = "Image Size Test",
		.engineVersion = 1,
		.apiVersion = VK_API_VERSION_1_0,
	};

	std::vector<const char*> instance_layers{
		"VK_LAYER_KHRONOS_validation"
	};
	std::vector<const char*> instance_extensions{};

	VkInstanceCreateInfo instanceCreateInfo{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pApplicationInfo = &applicationInfo,
		.enabledLayerCount = static_cast<uint32_t>(instance_layers.size()),
		.ppEnabledLayerNames = instance_layers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(instance_extensions.size()),
		.ppEnabledExtensionNames = instance_extensions.data()
	};

	VkInstance instance;
	if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
		std::cerr << "vkCreateInstance failed!";
		return EXIT_FAILURE;
	}

	volkLoadInstance(instance);

	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

	for (auto physicalDevice : physicalDevices) {

		std::vector<const char*> device_layers{
			"VK_LAYER_KHRONOS_validation"
		};
		std::vector<const char*> device_extensions{};

		std::vector<float> queuePriorities = { 1.0f };

		std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos{ {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = 0,
			.queueCount = static_cast<uint32_t>(queuePriorities.size()),
			.pQueuePriorities = queuePriorities.data()
		} };

		VkPhysicalDeviceFeatures physicalDeviceFeatures{};
		physicalDeviceFeatures.sparseBinding = VK_TRUE;
		physicalDeviceFeatures.sparseResidencyImage3D = VK_TRUE;

		VkDeviceCreateInfo deviceCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size()),
			.pQueueCreateInfos = deviceQueueCreateInfos.data(),
			.enabledLayerCount = static_cast<uint32_t>(device_layers.size()),
			.ppEnabledLayerNames = device_layers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size()),
			.ppEnabledExtensionNames = device_extensions.data(),
			.pEnabledFeatures = &physicalDeviceFeatures,
		};

		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		std::cout << "Creating device: " << physicalDeviceProperties.deviceName << std::endl;

		// NVIDIA version scheme
		if (physicalDeviceProperties.vendorID == 4318) {
			std::cout << "NVIDIA Driver version: " <<
				((physicalDeviceProperties.driverVersion >> 22) & 0x3ff) << "." <<
				((physicalDeviceProperties.driverVersion >> 14) & 0x0ff) << "." <<
				((physicalDeviceProperties.driverVersion >> 6) & 0x0ff) << "." <<
				((physicalDeviceProperties.driverVersion) & 0x003f) << std::endl;
		}
		else {
			// standard Vulkan versioning
			std::cout << "Driver version: " <<
				(physicalDeviceProperties.driverVersion >> 22) << "." <<
				((physicalDeviceProperties.driverVersion >> 12) & 0x3ff) << "." <<
				(physicalDeviceProperties.driverVersion & 0xfff);
		}

		VkDevice device;
		if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
			std::cerr << "vkCreateDevice failed!";
			return EXIT_FAILURE;
		}

		VkFormat format = VK_FORMAT_R8_UNORM;
		VkImageType imageType = VK_IMAGE_TYPE_3D;
		VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageCreateFlags flags = VK_IMAGE_CREATE_SPARSE_BINDING_BIT | VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT;

		VkImageFormatProperties imageFormatProperties;
		if (vkGetPhysicalDeviceImageFormatProperties(physicalDevice, format, imageType, tiling, usage, flags, &imageFormatProperties) != VK_SUCCESS) {
			std::cerr << "vkGetPhysicalDeviceImageFormatProperties!";
			return EXIT_FAILURE;
		}

		VkPhysicalDeviceProperties2 physicalDeviceProperties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR };
		vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties2);

		std::cout << "VkImageFormatPropeties.maxExtent = { " <<
			imageFormatProperties.maxExtent.width << ", " <<
			imageFormatProperties.maxExtent.height << ", " <<
			imageFormatProperties.maxExtent.depth << " }" << std::endl;

		std::cout << "VkPhysicalDeviceFormatProperties2.properties.limits.maxImageDimension3D = " <<
			physicalDeviceProperties2.properties.limits.maxImageDimension3D << std::endl;

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

		std::cout << "Creating image with extent: { " <<
			imageCreateInfo.extent.width << ", " <<
			imageCreateInfo.extent.height << ", " <<
			imageCreateInfo.extent.depth << " }" << std::endl;

		VkImage image;
		auto result = vkCreateImage(device, &imageCreateInfo, nullptr, &image);
		if (result == VK_SUCCESS) {
			std::cout << "vkCreateImage succeeded";
		}
		if (result == VK_ERROR_OUT_OF_DEVICE_MEMORY) {
			std::cerr << "vkCreateImage returned VK_ERROR_OUT_OF_DEVICE_MEMORY";
		}
	}
	return EXIT_SUCCESS;
}

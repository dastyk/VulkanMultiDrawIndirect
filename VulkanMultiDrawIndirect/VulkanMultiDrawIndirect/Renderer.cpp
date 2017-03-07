#include "Renderer.h"
#undef min
#undef max
#include <array>
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



Renderer::Renderer(HWND hwnd, uint32_t width, uint32_t height):_width(width), _height(height)
{

	/************Create Instance*************/
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_core_validation"
	};

	const auto vkAppInfo = &VulkanHelpers::MakeApplicationInfo(
		"Vulkan MDI",
		VK_MAKE_VERSION(1, 0, 0),
		"Frengine",
		VK_MAKE_VERSION(2, 0, 0)
	);
	std::vector<const char*> extensions = { "VK_KHR_surface", "VK_KHR_win32_surface", VK_EXT_DEBUG_REPORT_EXTENSION_NAME };
	const auto vkInstCreateInfo = VulkanHelpers::MakeInstanceCreateInfo(
		0,
		vkAppInfo,
		validationLayers.size(),
		validationLayers.data(),
		nullptr,
		extensions.size(),
		extensions.data()
	);
	VulkanHelpers::CreateInstance(&vkInstCreateInfo, &_instance);

	/*Create debug callback*/
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = VulkanHelpers::debugCallback;

	if (VulkanHelpers::CreateDebugReportCallbackEXT(_instance, &createInfo, nullptr, &_debugCallback) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug callback!");
	}
	
	/***********Enumerate physical devices*************/
	_devices = VulkanHelpers::EnumeratePhysicalDevices(_instance);
	if (_devices.size() == 0)
		throw std::runtime_error("No devices, pesant");


	/***************Make sure the device has a queue that can handle rendering*****************/
	auto queueFamInfo = VulkanHelpers::EnumeratePhysicalDeviceQueueFamilyProperties(_instance);
	size_t queueIndex = -1;
	
	for (uint32_t i = 0; i <  queueFamInfo[0].size(); i++)
	{
		if (queueFamInfo[0][i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			queueIndex = i;
			break;
		}
	}
	if (queueIndex == -1)
		throw std::runtime_error("No queue can render");



	/*************Create the device**************/
	auto queueInfo = VulkanHelpers::MakeDeviceQueueCreateInfo(queueIndex, 1);
	auto lInfo = VulkanHelpers::MakeDeviceCreateInfo(1, &queueInfo);
	VulkanHelpers::CreateLogicDevice(_devices[0], &lInfo, &_device);

	// Get the queue
	vkGetDeviceQueue(_device, queueIndex, 0, &_queue);

	// Create command pool
	auto cmdPoolInfo = VulkanHelpers::MakeCommandPoolCreateInfo(queueIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	VulkanHelpers::CreateCommandPool(_device, &cmdPoolInfo, &_cmdPool);

	// Allocate cmd buffer
	VulkanHelpers::AllocateCommandBuffers(_device, &_cmdBuffer, _cmdPool);



	_CreateSurface(hwnd);
	_CreateSwapChain();


}

Renderer::~Renderer()
{
	vkDestroyCommandPool(_device, _cmdPool, nullptr);
	vkDestroyDevice(_device, nullptr);
	VulkanHelpers::DestroyDebugReportCallbackEXT(_instance, _debugCallback, nullptr);
	vkDestroyInstance(_instance, nullptr);
}

void Renderer::Render(void)
{

}

Texture2D * Renderer::CreateTexture(const char * path)
{
	int imageWidth, imageHeight, imageChannels;
	stbi_uc* imagePixels = stbi_load(path, &imageWidth, &imageHeight, &imageChannels, STBI_rgb_alpha);
	if (!imagePixels)
		throw std::runtime_error(std::string("Could not load image: ").append(path));

	VkDeviceSize imageSize = imageWidth * imageHeight * 4;

	VkImage stagingImage;
	VkDeviceMemory stagingMemory;

	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.extent.width = imageWidth;
	imageCreateInfo.extent.height = imageHeight;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.flags = 0;

	VulkanHelpers::CreateImage(_device, &imageCreateInfo, &stagingImage, nullptr); //Throws if failed

	VkMemoryRequirements memoryRequirement;
	vkGetImageMemoryRequirements(_device, stagingImage, &memoryRequirement);

	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(_devices[0], &memoryProperties); 

	VkMemoryPropertyFlags desiredProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	int32_t memoryTypeIndex = -1;
	for (int32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		/*From the documentation:
		memoryTypeBits is a bitmask and contains one bit set for every supported memory type for the resource.
		Bit i is set if and only if the memory type i in the VkPhysicalDeviceMemoryProperties structure for the physical device is supported for the resource.*/
		if ((memoryRequirement.memoryTypeBits & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & desiredProperties) == desiredProperties)
		{
			memoryTypeIndex = i;
			break;
		}
	}

	if (memoryTypeIndex < 0)
		throw std::runtime_error("Failed to find compatible memory type");

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirement.size;
	memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

	if (vkAllocateMemory(_device, &memoryAllocateInfo, nullptr, &stagingMemory) != VK_SUCCESS)
		throw std::runtime_error(std::string("Could not allocate memory for staging image: ").append(path));

	if(vkBindImageMemory(_device, stagingImage, stagingMemory, 0) != VK_SUCCESS)
		throw std::runtime_error(std::string("Could not bind memory to staging image: ").append(path));

	void* data;
	vkMapMemory(_device, stagingMemory, 0, imageSize, 0, &data);

	VkImageSubresource imageSubresource = {};
	imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageSubresource.mipLevel = 0;
	imageSubresource.arrayLayer = 0;

	VkSubresourceLayout subresourceLayout;
	vkGetImageSubresourceLayout(_device, stagingImage, &imageSubresource, &subresourceLayout);

	//If there's no padding issues, just fill it
	if (subresourceLayout.rowPitch == imageWidth * 4)
	{
		memcpy(data, imagePixels, imageSize);
	}
	else
	{
		//Deal with padding
		uint8_t* bytes = reinterpret_cast<uint8_t*>(data);
		for (int row = 0; row < imageHeight; row++)
		{
			memcpy(&bytes[row * subresourceLayout.rowPitch], &imagePixels[row * imageWidth * 4], imageWidth * 4);
		}
	}

	vkUnmapMemory(_device, stagingMemory);
	stbi_image_free(imagePixels);

	Texture2D* texture = new Texture2D();
	VulkanHelpers::CreateImage2D(_device, &(texture->_image), imageWidth, imageHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	VulkanHelpers::AllocateImageMemory(_device, _devices[0], texture->_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &(texture->_memory));
	vkBindImageMemory(_device, texture->_image, texture->_memory, 0);

	//VulkanHelpers::BeginCommandBuffer(_cmdBuffer,VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	//VulkanHelpers::TransitionImageLayout(_device, stagingImage, _cmdBuffer, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	//VulkanHelpers::TransitionImageLayout(_device, texture->_image, _cmdBuffer, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	//VkImageSubresourceLayers srl = {};
	//srl.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//srl.baseArrayLayer = 0;
	//srl.mipLevel = 0;
	//srl.layerCount = 1;

	//VkImageCopy region = {};
	//region.srcSubresource = srl;
	//region.dstSubresource = srl;
	//region.srcOffset = { 0,0,0 };
	//region.dstOffset = { 0,0,0 };
	//region.extent.width = imageWidth;
	//region.extent.height = imageHeight;
	//region.extent.depth = 1; 

	//vkCmdCopyImage(_cmdBuffer, stagingImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, texture->_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	//VulkanHelpers::TransitionImageLayout(_device, texture->_image, _cmdBuffer, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	//VulkanHelpers::EndCommandBuffer(_cmdBuffer);

	//VkSubmitInfo submitInfo = {};
	//submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//submitInfo.commandBufferCount = 1;
	//submitInfo.pCommandBuffers = &_cmdBuffer;
	//vkQueueSubmit(_queue, 1, &submitInfo, VK_NULL_HANDLE);

	
	
	

	return nullptr;
}

const void Renderer::_CreateSurface(HWND hwnd)
{
	/**************** Set up window surface *******************/
	TCHAR cname[256];
	GetClassName(hwnd, cname, 256);
	WNDCLASS wc;
	GetClassInfo(GetModuleHandle(NULL), cname, &wc);

	VkWin32SurfaceCreateInfoKHR wndCreateInfo;
	wndCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	wndCreateInfo.hwnd = hwnd;
	wndCreateInfo.hinstance = wc.hInstance;
	wndCreateInfo.flags = 0;
	wndCreateInfo.pNext = nullptr;
	
	if (vkCreateWin32SurfaceKHR(_instance, &wndCreateInfo, nullptr, &_surface) != VK_SUCCESS)
		throw std::runtime_error("Window surface creation failed.");

	// For validation purposes
	VkBool32 surfaceSupported;
	vkGetPhysicalDeviceSurfaceSupportKHR(_devices[0], 0, _surface, &surfaceSupported);
	if (surfaceSupported == VK_FALSE)
	{
		throw std::runtime_error("Surface is not supported for the physical device!");
	}

}

const void Renderer::_CreateSwapChain()
{
	/************** Set up swap chain ****************/
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> supportedFormats;
	std::vector<VkPresentModeKHR> supportedPresentModes;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_devices[0], _surface, &capabilities);


	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(_devices[0], _surface, &formatCount, nullptr);
	supportedFormats.resize(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(_devices[0], _surface, &formatCount, supportedFormats.data());

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(_devices[0], _surface, &presentModeCount, nullptr);
	supportedPresentModes.resize(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(_devices[0], _surface, &presentModeCount, supportedPresentModes.data());

	VkSurfaceFormatKHR bestFormat;
	if (supportedFormats.size() == 1 && supportedFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		bestFormat = supportedFormats[0];
	}
	else
	{
		//Settle for first format unless something better comes along
		bestFormat = supportedFormats[0];
		for (const auto& i : supportedFormats)
		{
			if (i.format == VK_FORMAT_B8G8R8A8_UNORM && i.format == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
			{
				bestFormat = i;
				break;
			}
		}
	}
	_swapchainFormat = bestFormat.format;
	//Unless something better comes along, use the standard mode
	VkPresentModeKHR bestPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& i : supportedPresentModes)
	{
		if (i == VK_PRESENT_MODE_MAILBOX_KHR)
			bestPresentMode = i;
	}

	VkExtent2D bestExtent = { _width, _height };
	bestExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, _width));
	bestExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, _height));
	_swapchainExtent = bestExtent;

	uint32_t imageCount = std::min(capabilities.minImageCount + 1, capabilities.maxImageCount);

	VkSwapchainCreateInfoKHR swapCreateInfo;
	ZeroMemory(&swapCreateInfo, sizeof(swapCreateInfo));
	swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapCreateInfo.surface = _surface;
	swapCreateInfo.minImageCount = imageCount;
	swapCreateInfo.imageFormat = bestFormat.format;
	swapCreateInfo.imageColorSpace = bestFormat.colorSpace;
	swapCreateInfo.imageExtent = bestExtent;
	swapCreateInfo.imageArrayLayers = 1;
	swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapCreateInfo.preTransform = capabilities.currentTransform;
	swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapCreateInfo.presentMode = bestPresentMode;
	swapCreateInfo.clipped = VK_TRUE;
	swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	//Assume graphics family is the same as present family
	swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapCreateInfo.queueFamilyIndexCount = 0;
	swapCreateInfo.pQueueFamilyIndices = nullptr;

	if (vkCreateSwapchainKHR(_device, &swapCreateInfo, nullptr, &_swapchain) != VK_SUCCESS)
		throw std::runtime_error("Failed to create swapchain");

	uint32_t swapchainImageCount = 0;
	if (vkGetSwapchainImagesKHR(_device, _swapchain, &swapchainImageCount, nullptr) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to get swapchain image count!");
	}
	_swapchainImages.resize(swapchainImageCount);
	if (vkGetSwapchainImagesKHR(_device, _swapchain, &swapchainImageCount, _swapchainImages.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to get swapchain images!");
	}

	_swapchainImageViews.resize(_swapchainImages.size());
	for (uint32_t i = 0; i < _swapchainImages.size(); ++i)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.pNext = nullptr;
		viewInfo.flags = 0;
		viewInfo.image = _swapchainImages[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = _swapchainFormat;
		viewInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(_device, &viewInfo, nullptr, &_swapchainImageViews[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create swapchain image view!");
	}
}

#include "Renderer.h"
#undef min
#undef max
#include <array>
#include <algorithm>

using namespace std;

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

	_CreateOffscreenImage();
	_CreateOffscreenImageView();
	_CreateRenderPass();
}

Renderer::~Renderer()
{
	vkDestroyRenderPass(_device, _renderPass, nullptr);
	vkDestroyImageView(_device, _offscreenImageView, nullptr);
	vkFreeMemory(_device, _offscreenImageMemory, nullptr);
	vkDestroyImage(_device, _offscreenImage, nullptr);
	vkDestroyCommandPool(_device, _cmdPool, nullptr);
	vkDestroyDevice(_device, nullptr);
	VulkanHelpers::DestroyDebugReportCallbackEXT(_instance, _debugCallback, nullptr);
	vkDestroyInstance(_instance, nullptr);
}

void Renderer::Render(void)
{

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

bool Renderer::_AllocateMemory(VkMemoryPropertyFlagBits desiredProps, const VkMemoryRequirements& memReq, VkDeviceMemory& memory)
{
	uint32_t memTypeBits = memReq.memoryTypeBits;
	VkDeviceSize memSize = memReq.size;

	VkPhysicalDeviceMemoryProperties memProps;
	vkGetPhysicalDeviceMemoryProperties(_devices[0], &memProps);
	for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i)
	{
		// Current memory type (i) suitable and the memory has desired properties.
		if ((memTypeBits & (1 << i)) && ((memProps.memoryTypes[i].propertyFlags & desiredProps) == desiredProps))
		{
			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.pNext = nullptr;
			allocInfo.allocationSize = memSize;
			allocInfo.memoryTypeIndex = i;

			VkResult result = vkAllocateMemory(_device, &allocInfo, nullptr, &memory);
			if (result == VK_SUCCESS)
			{
				return true;
			}
		}
	}

	return false;
}

void Renderer::_CreateOffscreenImage(void)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext = nullptr;
	imageInfo.flags = 0;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageInfo.extent = { _swapchainExtent.width, _swapchainExtent.height, 1 };
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VkResult result = vkCreateImage(_device, &imageInfo, nullptr, &_offscreenImage);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create offscreen image!");
	}

	VkMemoryRequirements memReq;
	vkGetImageMemoryRequirements(_device, _offscreenImage, &memReq);

	if (!_AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memReq, _offscreenImageMemory))
	{
		throw runtime_error("Failed to allocate memory for offscreen image!");
	}

	result = vkBindImageMemory(_device, _offscreenImage, _offscreenImageMemory, 0);
	if (result != VK_SUCCESS)
	{
		throw runtime_error("Failed to bind offscreen image to memory!");
	}
}

void Renderer::_CreateOffscreenImageView(void)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.pNext = nullptr;
	viewInfo.flags = 0;
	viewInfo.image = _offscreenImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	viewInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY , VK_COMPONENT_SWIZZLE_IDENTITY };
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkResult result = vkCreateImageView(_device, &viewInfo, nullptr, &_offscreenImageView);
	if (result != VK_SUCCESS)
	{
		throw runtime_error("Failed to create offscreen image view!");
	}
}

void Renderer::_CreateRenderPass(void)
{
	array<VkAttachmentDescription, 1> attachments = {};
	attachments[0].flags = 0;
	attachments[0].format = VK_FORMAT_R8G8B8A8_UNORM;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

	VkAttachmentReference colorRef = {};
	colorRef.attachment = 0;
	colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.flags = 0;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = nullptr;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorRef;
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = nullptr;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	VkRenderPassCreateInfo passInfo = {};
	passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	passInfo.pNext = nullptr;
	passInfo.flags = 0;
	passInfo.attachmentCount = attachments.size();
	passInfo.pAttachments = attachments.data();
	passInfo.subpassCount = 1;
	passInfo.pSubpasses = &subpass;
	passInfo.dependencyCount = 0;
	passInfo.pDependencies = nullptr;

	VkResult result = vkCreateRenderPass(_device, &passInfo, nullptr, &_renderPass);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create render pass!");
	}
}

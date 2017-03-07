#include "Renderer.h"
#undef min
#undef max
#include <array>
#include <algorithm>



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

const void Renderer::CreateMesh()
{
	return void();
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

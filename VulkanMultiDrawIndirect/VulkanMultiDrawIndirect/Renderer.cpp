#include "Renderer.h"
#include "VulkanHelpers.h"
#include <array>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData) {

	printf("validation layer: %s\n", msg);
	return VK_FALSE;
}


Renderer::Renderer()
{
	auto appInfo = VulkanHelpers::MakeApplicationInfo("MDIProj");
	auto info = VulkanHelpers::MakeInstanceCreateInfo(
		0, &appInfo);

	/************Create Instance*************/
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_core_validation"
	};

	const auto vkAppInfo = &VulkanHelpers::MakeApplicationInfo(
		"Vulkan Renderer",
		VK_MAKE_VERSION(1, 0, 0),
		"Frengine",
		VK_MAKE_VERSION(1, 0, 0)
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
	VulkanHelpers::CreateInstance(&info, &_instance);

	/*Create debug callback*/
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	if (vkCreateDebugReportCallbackEXT(_instance, &createInfo, nullptr, &_debugCallback) != VK_SUCCESS) {
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



}

Renderer::~Renderer()
{
	vkDestroyCommandPool(_device, _cmdPool, nullptr);
	vkDestroyDevice(_device, nullptr);
	vkDestroyDebugReportCallbackEXT(_instance, _debugCallback, nullptr);
	vkDestroyInstance(_instance, nullptr);
}

void Renderer::Render(void)
{

}

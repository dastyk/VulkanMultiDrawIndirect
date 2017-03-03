#include "Renderer.h"
#include "VulkanHelpers.h"

Renderer::Renderer()
{
	auto appInfo = VulkanHelpers::MakeApplicationInfo("MDIProj");
	auto info = VulkanHelpers::MakeInstanceCreateInfo(
		0, &appInfo);

	VulkanHelpers::CreateInstance(&info, &_instance);


	_devices = VulkanHelpers::EnumeratePhysicalDevices(_instance);
	if (_devices.size() == 0)
		throw std::runtime_error("No devices, pesant");

	auto queueInfo = VulkanHelpers::MakeDeviceQueueCreateInfo(0, 1);
	auto lInfo = VulkanHelpers::MakeDeviceCreateInfo(1, &queueInfo);
	VulkanHelpers::CreateLogicDevice(_devices[0], &lInfo, &_device);





}

Renderer::~Renderer()
{

}

void Renderer::Render(void)
{

}

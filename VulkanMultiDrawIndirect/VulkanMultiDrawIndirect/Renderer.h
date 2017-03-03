#pragma once
#include <vulkan\vulkan.h>
#include <vector>

#pragma comment(lib, "vulkan-1.lib")

class Renderer
{
public:
	Renderer();
	~Renderer();
	void Render(void);

private:
	VkInstance _instance;
	std::vector<VkPhysicalDevice> _devices;
	VkDevice _device;
};

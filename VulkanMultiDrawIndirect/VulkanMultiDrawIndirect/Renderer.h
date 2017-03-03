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


	const void/*Mesh**/ CreateMesh(/*MeshData*/);
	const void/*Texture2D*/  CreateTexture(const char* path);
	const void Submit(/*Mesh*/);
	const void Unsubmit(/*Mesh*/);

private:
	VkInstance _instance;
	std::vector<VkPhysicalDevice> _devices;
	VkDevice _device;
	VkCommandPool _cmdPool;
	VkCommandBuffer _cmdBuffer;
	VkQueue _queue;
	VkDebugReportCallbackEXT _debugCallback;
};

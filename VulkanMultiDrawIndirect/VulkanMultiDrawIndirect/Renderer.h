#pragma once
#include "VulkanHelpers.h"
#include <vector>
#include <Parsers.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <unordered_map>
#include "GPUTimer.h"
#include "Texture2D.h"
#include "VertexBufferHandler.h"

#pragma comment(lib, "vulkan-1.lib")

class Renderer
{
public:
	typedef uint32_t MeshHandle;

	enum class RenderStrategy
	{
		Traditional,
		IndirectRecord,
		IndirectResubmit
	};

public:
	Renderer(HWND hwnd, uint32_t width, uint32_t height);
	~Renderer();
	void Render(void);


	MeshHandle CreateMesh(const std::string& file);
	Texture2D*  CreateTexture(const char* path);
	const void Submit(MeshHandle mesh);
	//const void Unsubmit(/*Mesh*/);

	void UseStrategy(RenderStrategy strategy);

private:
	void _RenderSceneTraditional(void);
	void _BlitSwapchain(void);
	const void _CreateSurface(HWND hwnd);
	const void _CreateSwapChain();
	void _CreateSemaphores(void);
	bool _AllocateMemory(VkMemoryPropertyFlagBits desiredProps, const VkMemoryRequirements& memReq, VkDeviceMemory& memory);
	void _CreateOffscreenImage(void);
	void _CreateOffscreenImageView(void);
	void _CreateDepthBufferImage(void);
	void _CreateDepthBufferImageView(void);
	void _CreateRenderPass(void);
	void _CreateFramebuffer(void);
	void _CreateShaders(void);
	void _CreateShader(const char* shaderCode, VkShaderModule& shader);
	void _CreateDescriptorStuff();



private:
	uint32_t _width;
	uint32_t _height;
	GPUTimer* _gpuTimer;

	VkInstance _instance;
	std::vector<VkPhysicalDevice> _devices;
	VkDevice _device;
	VkCommandPool _cmdPool;
	VkCommandBuffer _cmdBuffer;
	VkCommandBuffer _blitCmdBuffer;
	VkQueue _queue;
	VkDebugReportCallbackEXT _debugCallback;
	VkSurfaceKHR _surface;
	VkFormat _swapchainFormat;
	VkExtent2D _swapchainExtent;
	VkSwapchainKHR _swapchain;
	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;

	std::vector<std::tuple<uint32_t, uint32_t, uint32_t, ArfData::Data>> _meshes;
	std::vector<MeshHandle> _renderMeshes; // The actual meshes to render during a frame

	VkDescriptorPool _descPool;
	VkDescriptorSetLayout _descLayout;
	VkDescriptorSet _descSet;

	std::unordered_map<std::string, Texture2D*> _textures;

	VkSemaphore _imageAvailable = VK_NULL_HANDLE;
	VkSemaphore _swapchainBlitComplete = VK_NULL_HANDLE;
	VkImage _offscreenImage = VK_NULL_HANDLE;
	VkDeviceMemory _offscreenImageMemory = VK_NULL_HANDLE;
	VkImageView _offscreenImageView = VK_NULL_HANDLE;
	VkImage _depthBufferImage = VK_NULL_HANDLE;
	VkDeviceMemory _depthBufferImageMemory = VK_NULL_HANDLE;
	VkImageView _depthBufferImageView = VK_NULL_HANDLE;
	VkRenderPass _renderPass = VK_NULL_HANDLE;
	VkFramebuffer _framebuffer = VK_NULL_HANDLE;
	VkShaderModule _vertexShader = VK_NULL_HANDLE;
	VkShaderModule _fragmentShader = VK_NULL_HANDLE;

	VertexBufferHandler* _vertexBufferHandler;

	RenderStrategy _strategy = RenderStrategy::Traditional;
	RenderStrategy _nextStrategy = RenderStrategy::Traditional;
};

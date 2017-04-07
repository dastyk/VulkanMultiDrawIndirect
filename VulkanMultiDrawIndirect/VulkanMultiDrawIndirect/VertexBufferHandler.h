#pragma once
#include "VulkanHelpers.h"
#include <map>
#include <tuple>
#include <vector>
#define TypeSize(x) (static_cast<uint16_t>(x) & 0x00ff)

enum class VertexType : uint16_t
{
	Position = ((1U << 0U) << 8U) | 16U,
	TexCoord = ((1U << 1U) << 8U) | 8U,
	Normal = ((1U << 2U) << 8U) | 16U,
	Translation = ((1U << 3U) << 8U) | 64U,
	IndirectBuffer = ((1U << 4U) << 8U) | 16U,
	Index = ((1U << 5U) << 8U) | 4U
};

static std::vector<std::tuple<VertexType, VkFormat>> Texels =
{
	{ VertexType::Position , VK_FORMAT_R32G32B32A32_SFLOAT },
	{ VertexType::TexCoord , VK_FORMAT_R32G32_SFLOAT },
	{ VertexType::Normal , VK_FORMAT_R32G32B32A32_SFLOAT }
};

class VertexBufferHandler
{

	struct BufferSet
	{
		VkDeviceMemory memory;
		VkBuffer buffer;
		VkBufferView view;
		uint32_t maxCount;
		uint32_t firstFree;
	};


public:
	VertexBufferHandler(VkPhysicalDevice phydev, VkDevice device, VkQueue queue, VkCommandBuffer cmdBuffer);
	~VertexBufferHandler();

	/* Creates a vertex buffer
	*  - return value is the offset in the memory*/
	const uint32_t CreateBuffer(void* data, uint32_t numElements, VertexType type);

	std::vector<VkDescriptorPoolSize> GetDescriptorPoolSizes();
	std::vector<VkDescriptorSetLayoutBinding> GetDescriptorSetLayoutBindings(uint32_t bindingOffset);
	void WriteDescriptorSets(VkDescriptorSet descSet, uint32_t bindingOffset);
	VkBuffer GetBuffer(VertexType type);

private:
	const void _CreateBufferSet(VertexType type, uint32_t maxElements);
private:
	std::map<VertexType, BufferSet>_bufferSets;

	VkPhysicalDevice _phydev;
	VkDevice _device;
	VkCommandBuffer _cmdBuffer;
	VkQueue _queue;

};


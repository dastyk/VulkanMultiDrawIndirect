#pragma once
#include "VulkanHelpers.h"
#include <map>
#define TypeSize(x) (static_cast<uint16_t>(x) & 0x00ff)

enum class VertexType : uint16_t
{
	Position = ((1U << 0U) << 8U) | 12U,
	TexCoord = ((1U << 1U) << 8U) | 8U,
	Normal = ((1U << 2U) << 8U) | 12U,
	Translation = ((1U << 2U) << 8U) | 64U
};
class VertexBufferHandler
{

	struct BufferSet
	{
		VkDeviceMemory memory;
		VkBuffer buffer;
		uint32_t maxCount;
		uint32_t firstFree;
	};


public:
	VertexBufferHandler(VkPhysicalDevice phydev, VkDevice device, VkQueue queue, VkCommandBuffer cmdBuffer);
	~VertexBufferHandler();

	/* Creates a vertex buffer
	*  - bytewidth is ex. sizeof(float)*3 if data is a list of positions
	*  - numElements is the number of positions
	*  - return value is the offset in the memory*/
	const uint32_t CreateBuffer(void* data, uint32_t numElements, VertexType type);

	std::vector<VkDescriptorBufferInfo> GetBufferInfo();

private:
	const void _CreateBufferSet(VertexType type);
private:
	std::map<VertexType, BufferSet>_bufferSets;

	VkPhysicalDevice _phydev;
	VkDevice _device;
	VkCommandBuffer _cmdBuffer;
	VkQueue _queue;

};


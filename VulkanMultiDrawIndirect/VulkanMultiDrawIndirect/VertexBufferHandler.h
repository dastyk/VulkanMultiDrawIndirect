#pragma once
#include "VulkanHelpers.h"
#include <map>

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
	const uint32_t CreateBuffer(void* data, uint32_t byteWidth, uint32_t numElements);
private:
	const void _CreateBufferSet(BufferSet& set, uint32_t byteWidth, uint32_t maxCount);
private:
	std::map<uint32_t, BufferSet>_bufferSets;

	VkPhysicalDevice _phydev;
	VkDevice _device;
	VkCommandBuffer _cmdBuffer;
	VkQueue _queue;

};


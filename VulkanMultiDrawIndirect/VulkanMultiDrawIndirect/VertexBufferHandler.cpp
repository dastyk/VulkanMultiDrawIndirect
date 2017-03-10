#include "VertexBufferHandler.h"



VertexBufferHandler::VertexBufferHandler(VkPhysicalDevice phydev, VkDevice device, VkQueue queue, VkCommandBuffer cmdBuffer) : _phydev(phydev), _device(device), _queue(queue), _cmdBuffer(cmdBuffer)
{
}


VertexBufferHandler::~VertexBufferHandler()
{
}

const uint32_t VertexBufferHandler::CreateBuffer(void * data, uint32_t byteWidth, uint32_t numElements)
{
	auto& find = _bufferSets.find(byteWidth);
	auto& bufferSet = _bufferSets[byteWidth];
	if (find == _bufferSets.end())
	{
		// Bufferset of this bytewidth not found, create it
		_CreateBufferSet(bufferSet, byteWidth, (100 MB) / byteWidth);
	}

	VkDeviceSize totalSize = byteWidth*numElements;
	if (bufferSet.firstFree + numElements > bufferSet.maxCount)
		throw std::runtime_error("Bufferset is full.");

	uint32_t offset = bufferSet.firstFree;

	/* Create a staging buffer*/
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
	VulkanHelpers::CreateBuffer(_phydev, _device, totalSize, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		&stagingBuffer, &stagingMemory);

	/* Copy data to staging buffer*/
	void* srcData;
	VulkanHelpers::MapMemory(_device, stagingMemory, &srcData, totalSize);
	memcpy(srcData, data, totalSize);
	vkUnmapMemory(_device, stagingMemory);
	
	/*Copy data to the actual buffer*/
	VulkanHelpers::BeginCommandBuffer(_cmdBuffer);
	VulkanHelpers::CopyDataBetweenBuffers(_cmdBuffer, stagingBuffer, 0, bufferSet.buffer, offset, totalSize);
	vkEndCommandBuffer(_cmdBuffer);
	auto& sInfo = VulkanHelpers::MakeSubmitInfo(1, &_cmdBuffer);
	VulkanHelpers::QueueSubmit(_queue, 1, &sInfo);
	vkQueueWaitIdle(_queue);
}

const void VertexBufferHandler::_CreateBufferSet(BufferSet & set, uint32_t byteWidth, uint32_t maxCount)
{
	return void();
}

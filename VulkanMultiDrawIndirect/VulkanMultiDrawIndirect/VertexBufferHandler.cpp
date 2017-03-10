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
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
	auto& stagingBufferInfo = VulkanHelpers::MakeBufferCreateInfo(totalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	VulkanHelpers::CreateBuffer(_phydev, _device, totalSize, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		&stagingBuffer, &stagingMemory);

	VulkanHelpers::CopyDataBetweenBuffers(_cmdBuffer, stagingBuffer, 0, bufferSet.buffer, offset, totalSize);

}

const void VertexBufferHandler::_CreateBufferSet(BufferSet & set, uint32_t byteWidth, uint32_t maxCount)
{
	return void();
}

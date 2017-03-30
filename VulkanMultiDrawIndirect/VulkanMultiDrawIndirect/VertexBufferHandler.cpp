#include "VertexBufferHandler.h"



VertexBufferHandler::VertexBufferHandler(VkPhysicalDevice phydev, VkDevice device, VkQueue queue, VkCommandBuffer cmdBuffer) : _phydev(phydev), _device(device), _queue(queue), _cmdBuffer(cmdBuffer)
{
	_CreateBufferSet(VertexType::Position);
	_CreateBufferSet(VertexType::TexCoord);
	_CreateBufferSet(VertexType::Normal);
	_CreateBufferSet(VertexType::Translation);
}


VertexBufferHandler::~VertexBufferHandler()
{
	for (auto& set : _bufferSets)
	{
		vkDestroyBuffer(_device, set.second.buffer, nullptr);
		vkFreeMemory(_device, set.second.memory, nullptr);
	}
}

const uint32_t VertexBufferHandler::CreateBuffer(void* data, uint32_t numElements, VertexType type)
{
	auto& bufferSet = _bufferSets[type];
	auto byteWidth = TypeSize(type);
	VkDeviceSize totalSize = byteWidth*numElements;
	if (bufferSet.firstFree + numElements > bufferSet.maxCount)
		throw std::runtime_error("Bufferset is full.");

	uint32_t offset = bufferSet.firstFree;
	bufferSet.firstFree += numElements;

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
	VulkanHelpers::CopyDataBetweenBuffers(_cmdBuffer, stagingBuffer, 0, bufferSet.buffer, offset*byteWidth, totalSize);
	vkEndCommandBuffer(_cmdBuffer);
	auto& sInfo = VulkanHelpers::MakeSubmitInfo(1, &_cmdBuffer);
	VulkanHelpers::QueueSubmit(_queue, 1, &sInfo);
	vkQueueWaitIdle(_queue);

	/* Free staging buffer*/
	vkDestroyBuffer(_device, stagingBuffer, nullptr);
	vkFreeMemory(_device, stagingMemory, nullptr);

	return offset;

}

std::vector<VkDescriptorBufferInfo> VertexBufferHandler::GetBufferInfo()
{
	std::vector<VkDescriptorBufferInfo> descBuffInfo;

	for (auto& set : _bufferSets)
	{
		auto& buff = set.second;
		descBuffInfo.push_back({
			buff.buffer,
			0,
			VK_WHOLE_SIZE
		});
	}

	return descBuffInfo;
}

const void VertexBufferHandler::_CreateBufferSet(VertexType type)
{
	auto& set = _bufferSets[type];
	auto byteWidth = TypeSize(type);
	set.maxCount = (100 MB) / byteWidth;
	set.firstFree = 0;
	VulkanHelpers::CreateBuffer(_phydev, _device, 100 MB,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&set.buffer, &set.memory);
}

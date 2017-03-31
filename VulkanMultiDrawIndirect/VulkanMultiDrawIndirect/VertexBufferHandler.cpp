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
		if(set.second.view != VK_NULL_HANDLE)
			vkDestroyBufferView(_device, set.second.view, nullptr);
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

std::vector<VkDescriptorPoolSize> VertexBufferHandler::GetDescriptorPoolSizes()
{
	std::vector<VkDescriptorPoolSize> p;
	uint32_t total = 0;

	for (auto& set : _bufferSets)
		total = set.second.view == VK_NULL_HANDLE ? total : total + 1;
	p.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, total });
	p.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (uint32_t)_bufferSets.size() - total });
	return p;
}

std::vector<VkDescriptorSetLayoutBinding> VertexBufferHandler::GetDescriptorSetLayoutBindings()
{
	std::vector<VkDescriptorSetLayoutBinding> b;
	for (auto& set : _bufferSets)
	{
		b.push_back({
			(uint32_t)b.size(),
			set.second.view == VK_NULL_HANDLE ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
			1,
			VK_SHADER_STAGE_VERTEX_BIT,
			nullptr
		});
	}
	
	return b;
}

void VertexBufferHandler::WriteDescriptorSets(VkDescriptorSet descSet)
{
	std::vector<VkWriteDescriptorSet> s;
	std::vector<VkDescriptorBufferInfo> dinfo;
	dinfo.reserve(_bufferSets.size() * 2);
	uint32_t i = 0;
	for (auto& set : _bufferSets)
	{
		if (set.second.view == VK_NULL_HANDLE)
		{
			dinfo.push_back({
				set.second.buffer,
				0,
				VK_WHOLE_SIZE
			});
			s.push_back(VulkanHelpers::MakeWriteDescriptorSet(descSet, i, 0, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, nullptr, &dinfo[dinfo.size() - 1], nullptr));
		}
		else
		{
			s.push_back(VulkanHelpers::MakeWriteDescriptorSet(descSet, i, 0, 1, VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, nullptr, nullptr, &set.second.view));
		}
		i++;
	}
	vkUpdateDescriptorSets(_device, s.size(), s.data(), 0, nullptr);
}

const void VertexBufferHandler::_CreateBufferSet(VertexType type)
{
	auto& set = _bufferSets[type];
	auto byteWidth = TypeSize(type);
	set.maxCount = (100 MB) / byteWidth;
	set.firstFree = 0;
	for (auto& t : Texels)
	{
		
		if (std::get<0>(t) == type)
		{
			VulkanHelpers::CreateBuffer(_phydev, _device, 100 MB,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&set.buffer, &set.memory);

			VulkanHelpers::CreateBufferView(_device, set.buffer, &set.view, std::get<1>(t));
			return;
		}


	
	}
	VulkanHelpers::CreateBuffer(_phydev, _device, 100 MB,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&set.buffer, &set.memory);
	
}

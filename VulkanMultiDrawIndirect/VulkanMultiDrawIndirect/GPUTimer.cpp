#include "GPUTimer.h"



GPUTimer::GPUTimer(VkDevice device, uint8_t queryLatency) : _device(device), _maxCounters(20), _queryLatency(queryLatency)
{
	auto& info = VulkanHelpers::MakeQueryPoolCreateInfo(VK_QUERY_TYPE_TIMESTAMP, _queryLatency*_maxCounters*2);
	VulkanHelpers::CreateQueryPool(_device, &info, &_pool);




}

GPUTimer::~GPUTimer()
{
	vkDestroyQueryPool(_device, _pool, nullptr);
}


const void GPUTimer::Start(VkCommandBuffer& buffer, uint64_t GUID)
{
	auto& find = _timers.find(GUID);
	auto& timer = _timers[GUID];
	if (find == _timers.end())
	{
		timer.currentFrame = 0;
		timer.start = new uint32_t[_queryLatency];
		timer.end = new uint32_t[_queryLatency];
		uint32_t i = (_timers.size() - 1) * 2;
		for (uint8_t k = 0; k < _queryLatency; k++)
		{

		}
		
		if (_timers.size() >= _maxCounters)
		{
			throw std::runtime_error("To many GPU timers, growing not supported.");
		}

	}
	else
	{
		vkCmdResetQueryPool(
			buffer,
			_pool,
			timer.start,
			2);
	}
	

	vkCmdWriteTimestamp(buffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, _pool, timer.start);



}

const void GPUTimer::End(VkCommandBuffer & buffer, uint64_t GUID)
{
	auto& timer = _timers[GUID];
	vkCmdWriteTimestamp(buffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, _pool, timer.end);
}

const void GPUTimer::GetTime(uint64_t GUID)
{
	return void();
}

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


const void GPUTimer::Start(VkCommandBuffer& buffer, VkPipelineStageFlagBits flags, uint64_t GUID)
{
	auto& find = _timers.find(GUID);
	auto& timer = _timers[GUID];
	if (find == _timers.end())
	{
		timer.currentFrame = 0;
		timer.currentTimeFrame = 0;
		timer.start = new uint32_t[_queryLatency];
		timer.end = new uint32_t[_queryLatency];
		uint32_t start = (_timers.size() - 1)*_queryLatency * 2;
		for (uint8_t k = 0; k < _queryLatency; k++)
		{
			uint32_t latOff = start + k * 2;
			timer.start[k] = latOff;
			timer.end[k] = latOff + 1;
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
			timer.start[timer.currentFrame],
			2);
	}
	

	vkCmdWriteTimestamp(buffer, flags, _pool, timer.start[timer.currentFrame]);



}

const void GPUTimer::End(VkCommandBuffer& buffer, VkPipelineStageFlagBits flags, uint64_t GUID)
{
	auto& timer = _timers[GUID];
	vkCmdWriteTimestamp(buffer, flags, _pool, timer.end[timer.currentFrame]);
	timer.currentTimeFrame = timer.currentFrame;
	timer.currentFrame = (timer.currentFrame + 1) % _queryLatency;
}

const double GPUTimer::GetTime(uint64_t GUID)
{
	auto& timer = _timers[GUID];
	uint64_t times[2];
	vkGetQueryPoolResults(_device, _pool, timer.start[timer.currentTimeFrame], 2, sizeof(uint64_t) * 2, times, 0, VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);


	return 0.0;
}

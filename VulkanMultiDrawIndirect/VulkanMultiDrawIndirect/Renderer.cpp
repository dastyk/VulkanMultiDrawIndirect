#include "Renderer.h"
#include "VulkanHelpers.h"

Renderer::Renderer()
{
	auto info = VulkanHelpers::MakeInstanceCreateInfo(
		0);
	VkInstance inst;
	VulkanHelpers::CreateInstance(&info, &inst);
}

Renderer::~Renderer()
{

}

void Renderer::Render(void)
{

}

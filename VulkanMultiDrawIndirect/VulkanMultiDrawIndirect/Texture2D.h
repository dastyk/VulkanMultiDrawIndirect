#ifndef _TEXTURE_2D_
#define _TEXTURE_2D_

#include <vulkan\vulkan.h>

struct Texture2D
{
public:
	VkImage _image;
	VkDeviceMemory _memory;
};


#endif

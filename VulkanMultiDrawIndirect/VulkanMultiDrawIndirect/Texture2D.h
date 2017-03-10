#ifndef _TEXTURE_2D_
#define _TEXTURE_2D_

#include <vulkan\vulkan.h>

struct Texture2D
{
public:
	VkImage _image = VK_NULL_HANDLE;
	VkImageView _imageView = VK_NULL_HANDLE;
	VkDeviceMemory _memory = VK_NULL_HANDLE;
};


#endif

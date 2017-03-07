#ifndef _TEXTURE_2D_
#define _TEXTURE_2D_

#include <vulkan\vulkan.h>

class Texture2D
{
	friend class Renderer;
public:
	Texture2D();
	~Texture2D();

private:
	VkImage _image;
	VkDeviceMemory _memory;
};


#endif

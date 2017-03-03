
#include <SDL.h>
#include "Scene.h"
#include <stdexcept>
#include <vulkan\vulkan.h>

int main(int argc, char** argv)
{

	try
	{

		if (SDL_Init(SDL_INIT_VIDEO) < 0)
			exit(1);

		Scene scene;
		scene.Init();
		scene.Start();
		scene.Shutdown();

	}
	catch (const std::runtime_error& err)
	{

	}
	return 0;
}
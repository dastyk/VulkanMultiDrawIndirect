
#include <SDL.h>
#include "Scene.h"
#include <stdexcept>

#include <SDL_syswm.h>
#define SCRWIDTH (800)
#define SCRHEIGHT (640)


int main(int argc, char** argv)
{

	try
	{

		if (SDL_Init(SDL_INIT_VIDEO) < 0)
			exit(1);
		SDL_Window* sdlWindow = nullptr;
		if ((sdlWindow = SDL_CreateWindow("Vulkan Project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCRWIDTH, SCRHEIGHT, SDL_WINDOW_SHOWN)) == nullptr)
			exit(2);

		//Stuff we need to pass to vulkan in order to create the swapchain:
		SDL_SysWMinfo winfo;
		SDL_VERSION(&winfo.version);
		SDL_GetWindowWMInfo(sdlWindow, &winfo);
		HWND hwnd = winfo.info.win.window; //This one

		TCHAR className[256];
		GetClassName(hwnd, className, 256);
		WNDCLASS wndClass;
		GetClassInfo(GetModuleHandle(NULL), className, &wndClass);
		//Vulkan needs "wndClass.hInstance" and "hwnd"

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
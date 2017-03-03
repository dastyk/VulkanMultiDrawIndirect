
#include <SDL.h>
#include "Scene.h"
#include <stdexcept>
#include <ConsoleThread.h>
#include <SDL_syswm.h>
#define SCRWIDTH (800)
#define SCRHEIGHT (640)

#pragma comment(lib, "DebugConsole.lib")

int main(int argc, char** argv)
{

	DebugUtils::DebugConsole::Command_Structure def =
	{
		nullptr,
		[](void* userData, int argc, char** argv) {printf("Command not found: %s\n\n", argv[0]);},
		[](void* userData, int argc, char** argv) {},
		"",
		""
	};

	DebugUtils::ConsoleThread::Init(&def);

	DebugUtils::DebugConsole::Command_Structure exitCommand =
	{
		nullptr,
		[](void* userData, int argc, char** argv) {
		SDL_Event user_event;

		user_event.type = SDL_QUIT;
		user_event.user.code = 0;
		user_event.user.data1 = NULL;
		user_event.user.data2 = NULL;
		SDL_PushEvent(&user_event);
	},
		[](void* userData, int argc, char** argv) {},
		"exit",
		"Terminates the program."
	};

	DebugUtils::ConsoleThread::AddCommand(&exitCommand);


	DebugUtils::ConsoleThread::ShowConsole();

	
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

		Renderer renderer(hwnd, SCRWIDTH, SCRHEIGHT);

		Scene scene(renderer);

		scene.Init();



		scene.Start();



		scene.Shutdown();

	}
	catch (const std::runtime_error& err)
	{
		printf(err.what());
		getchar();
	}


	DebugUtils::ConsoleThread::Shutdown();
	return 0;
}
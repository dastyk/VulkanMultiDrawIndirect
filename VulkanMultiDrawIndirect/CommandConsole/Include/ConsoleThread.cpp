#include "ConsoleThread.h"
#include <Windows.h>
#include <iostream>


namespace DebugUtils
{

	ConsoleThread* ConsoleThread::_instance = nullptr;

	ConsoleThread::ConsoleThread(const DebugConsole::Command_Structure* defaultCommandFunctions)
	{
		_console.Init(defaultCommandFunctions);
		DebugConsole::Command_Structure hideConsole =
		{
			&_running,
			[](void* userData, int argc, char** argv) {*static_cast<bool*>(userData) = false; },
			nullptr,
			"hide",
			"Hides the console window."
		};
		_console.AddCommand(&hideConsole);
	}


	ConsoleThread::~ConsoleThread()
	{
		_running = false;
		_thread.join();
		_console.Shutdown();
		
	}

	const void ConsoleThread::_Run()
	{
		char buffer[256];
		while (_running)
		{
			std::cin.getline(buffer, 256);
			_console.InterpretCommand(buffer);
		}
		FreeConsole();
		return void();
	}

	const void ConsoleThread::Init(const DebugConsole::Command_Structure * defaultCommandFunctions)
	{
		if (!_instance)
			_instance = new ConsoleThread(defaultCommandFunctions);


		return void();
	}

	const void ConsoleThread::Shutdown()
	{
		
		delete _instance;
		_instance = nullptr;
	}

	const void ConsoleThread::ShowConsole()
	{
		if (AllocConsole())
		{
			freopen("conin$", "r", stdin);
			freopen("conout$", "w", stdout);
			freopen("conout$", "w", stderr);

			printf("<----||Console Initialized||---->\n\n");
		}

		_instance->_running = true;
		_instance->_thread = std::thread(&ConsoleThread::_Run, _instance);

		SetThreadPriority(_instance->_thread.native_handle(), THREAD_MODE_BACKGROUND_BEGIN);
	}

	const void ConsoleThread::AddCommand(const DebugConsole::Command_Structure * command)
	{
		_instance->_console.AddCommand(command);
		return void();
	}

	const void ConsoleThread::AddCommand(uint32_t identifier, const DebugConsole::Command_Structure * command)
	{
		_instance->_console.AddCommand(identifier, command);
		return void();
	}

	
}
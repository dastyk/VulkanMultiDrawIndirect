
#ifndef _CONSOLE_THREAD_H_
#define _CONSOLE_THREAD_H_
#pragma once
#include <thread>
#include "Console.h"
namespace DebugUtils
{
	class ConsoleThread
	{
	private:
		static ConsoleThread* _instance;
		ConsoleThread(const DebugConsole::Command_Structure* defaultCommandFunctions);
		~ConsoleThread();
		DebugConsole _console;
		bool _running;
		std::thread _thread;
		const void _Run();
	public:
		/* Initiates the DebugConsole
		* Parameter is the default function to call if a interpreted command does not exist.
		*/
		static const void Init(const DebugConsole::Command_Structure* defaultCommandFunctions);
		/* Shutdown the DebugConsole*/
		static const void Shutdown();
		/*Show the DebugConsole*/
		static const void ShowConsole();
		/* Add a new command to the DebugConsole
		* First parameter is the identifier, second parameter is the function to call.
		*/
		static const void AddCommand(const DebugConsole::Command_Structure* command);
		/* Add a new command to the DebugConsole
		* First parameter is the identifier, second parameter is the function to call.
		* Pre-hashed identifier.
		*/
		static const void AddCommand(uint32_t indentifer, const DebugConsole::Command_Structure* command);
		/* Interpret a command and dispatch the correct function.
		* Parameter is the command, with args.
		*/
	};
}
#endif
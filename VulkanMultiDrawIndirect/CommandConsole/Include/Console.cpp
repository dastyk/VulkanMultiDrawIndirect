#include "Console.h"

namespace DebugUtils
{

	DebugConsole::DebugConsole()
	{
	}


	DebugConsole::~DebugConsole()
	{
	}

	/*Copies the default Command Functions to the 0 slot in the commands map*/
	const void DebugConsole::Init(const Command_Structure* defaultCommandFunctions)
	{
#ifdef _DEBUG
		if (defaultCommandFunctions->commandFunction == nullptr)
			throw std::exception("Invalid command function pointer.");
		if (defaultCommandFunctions->commandHelpFunction == nullptr)
			throw std::exception("Invalid command help function pointer.");
#endif
		_commands[0] = *defaultCommandFunctions;
		
		Command_Structure commandsCmd = 
		{
			this,
			[](void* userData, int argc, char** argv) {
			for (auto& c : static_cast<DebugConsole*>(userData)->_commands)
			{
				printf("%s - %s\n", c.second.name, c.second.description);
			}
		},
			[](void* userData, int argc, char** argv) {printf("%s", argv[0]); },
			"commands",
			"Lists all available commands\n"
		};

		AddCommand(&commandsCmd);


		return void();
	}
	const void DebugConsole::Shutdown()
	{
		return void();
	}
	/*Hash the identifier and add the command*/
	const void DebugConsole::AddCommand(const Command_Structure * command)
	{

		uint32_t hash = std::hash<std::string>{}(command->name);

#ifdef _DEBUG
		if (command->commandFunction == nullptr)
			throw std::exception("Invalid command function pointer.");

#endif
		
			
		_commands[hash] = *command;
		if (command->commandHelpFunction == nullptr)
			_commands[hash].commandHelpFunction = _commands[0].commandHelpFunction;
		
		return void();
	}
	/* Adds the command with the already hashed identifer*/
	const void DebugConsole::AddCommand(uint32_t identifier, const Command_Structure * command)
	{
#ifdef _DEBUG
		if (command->commandFunction == nullptr)
			throw std::exception("Invalid command function pointer.");

#endif


		_commands[identifier] = *command;
		if (command->commandHelpFunction == nullptr)
			_commands[identifier].commandHelpFunction = _commands[0].commandHelpFunction;


		return void();
	}

	/*Splits the command and calls the correct command function */
	const void DebugConsole::InterpretCommand(char * command)
	{

		int argc = 0;
		char* argv[MAX_ARGUMENTS];
		_ParseCommandString(command, argc, argv);


		if (argc > 0)
		{
			uint32_t hash = std::hash<std::string>{}(argv[0]);

			auto find = _commands.find(hash);
			if (find != _commands.end()) {

				if (GetArg("-h", nullptr, argc, argv)) {
					argv[0] = find->second.description;
					find->second.commandHelpFunction(find->second.userData, argc, argv);
					return;
				}
				find->second.commandFunction(find->second.userData, argc, argv);
			}
			else
			{
				_commands[0].commandFunction(argv[0], argc, argv);
			}
		}

		return void();
	}
	/* Split the command string into the arg array*/
	const void DebugConsole::_ParseCommandString(char* command, int& argc, char** argv)
	{

		argc = 0;
		int j = 0;
		int i = 0;
		char c = command[i];
		while (c != '\0')
		{
			while (!(c == ' ' || c == '\0'))
			{
				i++;
				c = command[i];
			}
			argv[argc] = &command[j];
			argc++;
			if (command[i] != '\0')
			{
				command[i] = '\0';
				i++;
				j = i;
				c = command[i];
				if (argc >= MAX_ARGUMENTS)
				{
					break;
				}
				
			}
			
			
		}

	}

	bool GetArg(char* arg, char** data, int argc, char** argv)
	{
		bool found = false;

		for (int i = 0; i < argc; i++)
		{
			if (std::string(arg) == std::string(argv[i]))
			{	
				if(data)
					*data = argv[i + 1];
				found = true;
				break;
			}
		}
		return found;
	}

}
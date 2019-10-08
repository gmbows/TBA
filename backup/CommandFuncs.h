#pragma once

#include "Command.h"

#include <vector>
#include <string>


	//===========
	//FUNCTION LIST
	//===========

	//Clear
	std::string clearFunc(Command* command, const std::vector<std::string> &);

	//Inventory
	std::string inventoryFunc(Command* command, const std::vector<std::string> &);

	//Move
	std::string moveFunc(Command* command, const std::vector<std::string> &);
	bool moveEC(Command* command, const std::vector<std::string>&);

	//Pause
	std::string pauseFunc(Command*, const std::vector<std::string>&);

	//Stop
	std::string stopFunc(Command*, const std::vector<std::string>&);

	//Help
	std::string helpFunc(Command*, const std::vector<std::string>&);
	bool helpEC(Command* command, const std::vector<std::string>&);

	//Unpause
	std::string unpauseFunc(Command*, const std::vector<std::string>&);

	//Target
	std::string targetFunc(Command*, const std::vector<std::string>&);
	bool targetEC(Command*, const std::vector<std::string>&);

	//Attack
	std::string attackFunc(Command*, const std::vector<std::string>&);
	bool attackEC(Command*, const std::vector<std::string>&);

	//Zoom
	std::string zoomFunc(Command*, const std::vector<std::string>&);
	bool zoomEC(Command*, const std::vector<std::string>&);

	//Say
	std::string sayFunc(Command*, const std::vector<std::string>&);
	bool sayEC(Command*, const std::vector<std::string>&);

	//Take
	std::string takeFunc(Command*, const std::vector<std::string>&);
	bool takeEC(Command*, const std::vector<std::string>&);

	//Hurtme
	std::string hurtmeFunc(Command*, const std::vector<std::string>&);
	bool hurtmeEC(Command*, const std::vector<std::string>&);

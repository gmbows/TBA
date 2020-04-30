#pragma once

#include "Command.h"

#include <vector>
#include <string>


	//===========
	//FUNCTION LIST
	//===========
	
	void checkHelp();

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

	//Hurtme
	std::string hurtmeFunc(Command*, const std::vector<std::string>&);
	bool hurtmeEC(Command*, const std::vector<std::string>&);

	//Exit
	std::string exitFunc(Command*, const std::vector<std::string>&);

	//Take
	std::string takeFunc(Command*, const std::vector<std::string>&);
	bool takeEC(Command*, const std::vector<std::string>&);

	//Select
	std::string selectFunc(Command*, const std::vector<std::string>&);
	bool selectEC(Command*, const std::vector<std::string>&);

	//Put
	std::string putFunc(Command*, const std::vector<std::string>&);
	bool putEC(Command*, const std::vector<std::string>&);

	//Search
	std::string searchFunc(Command*, const std::vector<std::string>&);
	bool searchEC(Command*, const std::vector<std::string>&);

	//Equip
	std::string equipFunc(Command*, const std::vector<std::string>&);
	bool equipEC(Command*, const std::vector<std::string>&);

	//Debug
	std::string debugFunc(Command*, const std::vector<std::string>&);

	//Examine
	std::string examineFunc(Command*, const std::vector<std::string>&);
	bool examineEC(Command*, const std::vector<std::string>&);

	//Help
	std::string helpFunc(Command*, const std::vector<std::string>&);
	bool helpEC(Command*, const std::vector<std::string>&);

	//Plant
	std::string plantFunc(Command*, const std::vector<std::string>&);
	bool plantEC(Command*, const std::vector<std::string>&);

	//Drink
	std::string drinkFunc(Command*, const std::vector<std::string>&);
	bool drinkEC(Command*, const std::vector<std::string>&);

	//Use
	std::string useFunc(Command*, const std::vector<std::string>&);
	bool useEC(Command*, const std::vector<std::string>&);

	//Giveme
	std::string givemeFunc(Command*, const std::vector<std::string>&);
	bool givemeEC(Command*, const std::vector<std::string>&);
	//Work
	std::string workFunc(Command*, const std::vector<std::string>&);
	bool workEC(Command*, const std::vector<std::string>&);

	//Goto
	std::string gotoFunc(Command*, const std::vector<std::string>&);
	bool gotoEC(Command*, const std::vector<std::string>&);

	//Control
	std::string controlFunc(Command*, const std::vector<std::string>&);
	bool controlEC(Command*, const std::vector<std::string>&);

	//Unequip
	std::string unequipFunc(Command*, const std::vector<std::string>&);
	bool unequipEC(Command*, const std::vector<std::string>&);

	//Server
	std::string serverFunc(Command*, const std::vector<std::string>&);
	bool serverEC(Command*, const std::vector<std::string>&);

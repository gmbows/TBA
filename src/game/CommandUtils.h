#pragma once
#include <vector>
#include <map>
#include <utility>

#include "GameObject.h"
#include "Command.h"
#include "../tools/Utility.h"

std::map<std::string,std::pair<std::vector<std::string>,std::vector<std::string>>> helpMap = {
	//command,{ {usage1,usage2},{returns}}
	{"help",{{"help","help [command]"},{"Displays command list","Displays command's usage and function"}}},
	{"clear",{{"clear"},{"Clears screen"}}},
	{"inventory",{{"inventory"},{"Displays player inventory"}}},
	{"search",{{"search"},{"Displays inventory of selected container"}}},
	{"examine",{{"examine","examine ☺w[item]☺","examine ☺w[partial item]☺"},{"Displays information about surroundings","Displays item info"}}},
	{"equip",{{"equip [item]","equip [partial item]"},{"Equips item"}}},
	{"take",{{"take [item name]","take ☺w[partial name]☺"},{"Takes item from selected container"}}},
	{"put",{{"put ☺w[item name]☺","put ☺w[partial item name]☺"},{"Puts item in selected container"}}},
	{"move",{{"move [direction]"},{"Moves player in direction"}}},
	{"pause",{{"pause"},{"Pauses game"}}},
	{"stop",{{"stop"},{"Halts player movement"}}},
	{"unpause",{{"unpause"},{"Unpauses game"}}},
	{"attack",{{"attack"},{"Engages player in combat with current target"}}},
	{"zoom",{{"zoom","zoom [in]","zoom [out]"},{"Changes or resets screen zoom level"}}},
	{"say",{{"say [text]"},{"Creates player speech"}}},
	{"exit",{{"exit"},{"Exits game"}}},
	{"select",{{"select [nearby object]","select [nearby character]"},{"Selects entity"}}},
	{"target",{{"target ☺g[character]☺"},{"Targets character"}}},
	{"plant",{{"plant ☺w[item name]☺","plant ☺w[partial item name]☺"},{"Plants seed"}}},
	{"drink",{{"drink ☺w[item name]☺","drink ☺w[partial item name]☺"},{"Drinks item"}}},
	{"use",{{"use ☺w[item name]☺","use ☺w[partial item name]☺"},{"Uses item"}}},
};

int moveItems(int itemCount,Item *goodItem,Inventory *source, Inventory *destination) {
	
	int taken;
	int index = source->find(goodItem->name);
	
	for(taken=0;taken<itemCount;taken++) {
		goodItem = source->remove(index);
		destination->add(goodItem);
		index = source->getFirstInstance(goodItem->name);
		if(index == -1) {
			//item not found in inventory
			taken++;
			break;
		} else if(index == -2) {
			debug(goodItem->name);
			break;
		}
	}
	return taken;
}

int findItemSource(std::string itemName,Inventory* &inv,std::string &sourceName) {

	Container *testContainer;

	int index = -1;

	//Search containers in area
	std::vector<GameObject*> containers = TBAGame->playerChar->getObjectsInRadius(OBJ_CONTAINER);

	for(int i=0;i<containers.size();i++) {
		testContainer = static_cast<Container*>(containers.at(i));
		index = testContainer->inventory->find(itemName);
		if(index >= 0) {
			inv = testContainer->inventory;
			sourceName = testContainer->getFormattedName();
		} else {
			inv = nullptr;
		}
	}
	return index;
}

std::string parseInteraction(Command *cmd, std::vector<std::string> args) {

	//Get command basename
	std::string command = cmd->aliases.at(0);

	//Set default itemcount
	int itemCount = 1;
	bool takeAll = false;
	bool pillage = false;
	std::string itemName;

	//Remove empty strings from args
	args = strip(args);

	//determine itemcount
	for(int i=0;i<args.size();i++) {
		if(isdigit(args.at(i))) {
			itemCount = std::stoi(strip(args.at(i)));
			args.erase(args.begin() + i);
			break;
		} else if(args.at(i) == "*") {
			if(i == 0) {
				pillage = true;
			} else {
				takeAll = true;
				args.erase(args.begin() + i);
			}
			break;
		}
	}

	if(args.size() == 0) return "\nNo item specified";

	//Join everything between [command,itemcount] into item name 
	itemName = join(' ',args);

	Inventory *source;
	Inventory *destination;

	std::string sourceName;
	std::string targetName;
	std::string tense,preposition;

	if(command == "take") {

		tense = "Took";
		preposition = "from";

		targetName = TBAGame->displayTarget->getFormattedName();

		//Source and destination containers
		// source is errorchecked and is a valid container

		source = TBAGame->displayTarget->getInventory();
		// sourceName = TBAGame->displayTarget->getFormattedName();
		destination = TBAGame->playerChar->inventory;
		
		sourceName = TBAGame->displayTarget->getFormattedName();

	} else if(command == "put") {

		// sourceName = "☺ginventory☺";
		targetName = TBAGame->displayTarget->getFormattedName();

		//Source and destination containers
		// source is errorchecked and is a valid container
		source = TBAGame->playerChar->inventory;
		destination = TBAGame->displayTarget->getInventory();

		if(TBAGame->displayTarget->type == OBJ_CHARACTER) {
			tense = "Gave";
			preposition = "to";
		} else {
			tense = "Put";
			preposition = "in";
		}
		
		sourceName = TBAGame->playerChar->getFormattedName();
	}

	if(pillage) {
		std::string items;
		std::vector<std::string> allItems = source->getContentString();
		for(int i=0;i<allItems.size();i++) {
			items += parseInteraction(cmd,{allItems.at(i),"*"});
		}
		return items;
	}

	//Check if item appears in selected container	
	int index = source->find(itemName);

	if(index >= 0) {
		//itemName was found in a container
		Item *goodItem;
		goodItem = source->getItem(index);
		if(takeAll) itemCount = source->itemCount(goodItem->id);
		int taken = moveItems(itemCount,goodItem,source,destination);
		if(taken > 1) return "\n"+tense+" "+goodItem->getFormattedName() + " ("+std::to_string(taken)+")"+" "+preposition+" "+targetName;
		return "\n"+tense+" "+goodItem->getFormattedName()+" "+preposition+" "+targetName;
	} else if(index == -2) {
		//itemName was found in a container but with multiple matches
		return "";
	} else {
		//itemName was not found in any containers
		return "\nItem ☺r"+itemName+"☺ not found in "+sourceName;
	}
}
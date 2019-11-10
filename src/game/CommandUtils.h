#pragma once
#include <vector>
#include <map>

#include "Command.h"
#include "../tools/Utility.h"

int moveItems(int itemCount,Item *goodItem,Inventory *source, Inventory *destination) {
	int taken = 0;
	int index = source->find(goodItem->name);

	for(taken;taken<itemCount;taken++) {
		goodItem = source->remove(index);
		destination->add(goodItem);
		index = source->find(goodItem->name);
		if(index == -1) {
			taken++;
			break;
		}
	}
	return taken;
}

int getSource(std::string itemName,Inventory* &inv,std::string &sourceName) {

	Container *testContainer;

	int index = -1;

	//Search containers in area
	std::vector<GameObject*> containers = TBAGame->playerChar->getObjectsInRadius(OBJ_CONTAINER);

	for(int i=0;i<containers.size();i++) {
		testContainer = static_cast<Container*>(containers.at(i));
		index = testContainer->inventory->find(itemName);
		if(index >= 0) {
			inv = testContainer->inventory;
			sourceName = testContainer->getName();
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
	std::string itemName;

	//Remove empty strings from args
	args = strip(args);

	//determine itemcount
	for(int i=0;i<args.size();i++) {
		if(isdigit(args.at(i))) {
			itemCount = std::stoi(strip(args.at(i)));
			args.erase(args.begin() + i);
		}
	}

	//Join everything between [command,itemcount] into item name 
	itemName = join(' ',args);

	if(command == "take") {

		//To retrieve matched item from source inventory
		Item *goodItem;

		//Source and destination containers
		Inventory *source;
		std::string sourceName;
		int index = getSource(itemName,source,sourceName);

		Inventory *destination = TBAGame->playerChar->inventory;

		if(index >= 0) {
			//itemName was found in a container
			goodItem = source->getItem(index);
			int taken = moveItems(itemCount,goodItem,source,destination);
			return "\nTook "+goodItem->name + " ("+std::to_string(taken)+")"+" from "+sourceName;
		} else if(index == -2) {
			//itemName was found in a container but with multiple matches
			return "";
		} else {
			//itemName was not found in any containers
			return "\nItem not found";
		}
	}
}
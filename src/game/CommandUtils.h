#pragma once
#include <vector>

#include "Command.h"
#include "../tools/Utility.h"

std::string parseInteraction(Command *cmd, std::vector<std::string> args) {
	std::string command = cmd->aliases.at(0);

	int itemcount = 1;
	std::string itemName;

	//determine itemcount
	for(int i=0;i<args.size();i++) {
		if(isdigit(args.at(i))) {
			itemcount = std::stoi(args.at(i));
			args.erase(args.begin() + i);
		}
	}

	itemName = join(' ',args);

	if(command == "take") {

		GameObject 	*testObject;
		Container 	*testContainer;
		Item		*goodItem;

		int index;

		//Search containers in area
		std::vector<GameObject*> objects = TBAGame->playerChar->getObjectsInRadius(OBJ_CONTAINER);

		for(int i=0;i<objects.size();i++) {
			testContainer = static_cast<Container*>(objects.at(i));
			index = testContainer->inventory->find(itemName);
			if(index >= 0) {
				goodItem = testContainer->inventory->remove(index);
				TBAGame->playerChar->inventory->add(goodItem);
				return "\nTook "+goodItem->name + " from "+testContainer->getName();
			} else if(index == -2) {
				return "";
			}
		}
		return "\nItem not found";
	}
}
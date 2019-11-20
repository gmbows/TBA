#include "CommandFuncs.h"
#include "../common/Common.h"
#include "../common/KeyFuncs.h"
#include "Character.h"
#include "Command.h"
#include "Container.h"
#include "FloatingText.h"
#include "CommandUtils.h"
#include "GameObject.h"

#include "World.h"

#include <vector>
#include <string>

#include <map>
#include <tuple>

//Dirmap
std::map<std::string,std::pair<std::string,std::tuple<int,int>>> dirMap = {

	{"north",{"North",std::make_tuple(0,1)}},
	{"south",{"South",std::make_tuple(0,-1)}},
	{"east",{"East",std::make_tuple(1,0)}},
	{"west",{"West",std::make_tuple(-1,0)}},
	{"up",{"North",std::make_tuple(0,1)}},
	{"down",{"South",std::make_tuple(0,-1)}},
	{"right",{"East",std::make_tuple(1,0)}},
	{"left",{"West",std::make_tuple(-1,0)}},
	{"n",{"North",std::make_tuple(0,1)}},
	{"s",{"South",std::make_tuple(0,-1)}},
	{"e",{"East",std::make_tuple(1,0)}},
	{"w",{"West",std::make_tuple(-1,0)}},
	{"northwest",{"Northwest",std::make_tuple(-1,1)}},
	{"southwest",{"Southwest",std::make_tuple(-1,-1)}},
	{"northeast",{"Northeast",std::make_tuple(1,1)}},
	{"southeast",{"Southeast",std::make_tuple(1,-1)}},
	{"nw",{"Northwest",std::make_tuple(-1,1)}},
	{"sw",{"Southwest",std::make_tuple(-1,-1)}},
	{"ne",{"Northeast",std::make_tuple(1,1)}},
	{"se",{"Southeast",std::make_tuple(1,-1)}},

};

//Clear
std::string clearFunc(Command* command,const std::vector<std::string>& args) {
	TBAGame->gameWindow->textScreen->content.clear();
	return "";
}

//Inventory
std::string inventoryFunc(Command* command,const std::vector<std::string>& args) {
	return "\n"+TBAGame->playerChar->getInvString();
}

//Move + EC
std::string moveFunc(Command* command, const std::vector<std::string> &args) {

	TBAGame->playerChar->direction = std::get<1>(dirMap.at(args.at(0)));
	TBAGame->playerChar->addStatus(STATUS_MOVE);
	return "\nMoving "+command->aux;

}
bool moveEC(Command* command, const std::vector<std::string>& args) {

	if(args.size() == 0 or dirMap.find(args.at(0)) == dirMap.end()) {
		command->error = "Invalid direction";
		return false;
	}
	command->aux = std::get<0>(dirMap.at(args.at(0)));
	return true;

}

//Pause
std::string pauseFunc(Command* command, const std::vector<std::string> &args) {
	
	TBAGame->togglePause();
	return (TBAGame->paused)? "\nPaused" : "\nUnpaused";

}

//Unpause
std::string unpauseFunc(Command* command, const std::vector<std::string> &args) {
	if(TBAGame->paused) {
		TBAGame->togglePause();
		return "\nUnpaused";
	}
	return "";
}

//Stop
std::string stopFunc(Command* command, const std::vector<std::string> &args) {
	
	TBAGame->playerChar->direction = {0,0};
	return "\nStopped moving";

}

//Help
std::string helpFunc(Command* command, const std::vector<std::string> &args) {

	return "\n"+join('\n',TBAGame->commandStrings);

}
bool helpEC(Command* command, const std::vector<std::string> &args) {

	if(args.size() > 0) {
		if(!contains(TBAGame->commandStrings,args.at(0))) {
			command->error = args.at(0)+": command not found";
			return false;
		}
	}
	return true;
}

//Target
std::string targetFunc(Command* command, const std::vector<std::string> &args) {
	//if no arguments were provided, find nearest character within 
	//player char's awareness radius
	if(args.size() == 0) {
		if(!TBAGame->playerChar->getNearestTarget()) return "\nNo valid targets found";
		return "\n"+TBAGame->playerChar->getName()+" targets "+TBAGame->playerChar->getTargetName();
	}

	//Otherwise, target object specified by aux string set in EC function
	//TBAGame->playerChar->setTarget(TBAGame->findObject(command->aux));	
	TBAGame->playerChar->findTargetInRadius(command->aux);
	return "\n"+TBAGame->playerChar->getName()+" targets "+TBAGame->playerChar->getTargetName();
}
bool targetEC(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		command->aux = "None";
		return true;
	}

	std::string targetName = join(' ',args);
	if(!TBAGame->playerChar->findTargetInRadius(targetName)) {
		command->error = "Target not found";
		return false;
	}

	command->aux = targetName;
	return true;

}

//Attack
std::string attackFunc(Command* command, const std::vector<std::string> &args) {
	TBAGame->playerChar->setStatus(STATUS_COMBAT);
	return "\nAttacking "+TBAGame->playerChar->getTargetName();
}
bool attackEC(Command* command, const std::vector<std::string> &args) {

	if(args.size() > 0) {
		std::string targetName = join(' ',args);
		Character* newTarget = static_cast<Character*>(TBAGame->findObject(targetName));
		if(newTarget == nullptr) {
			command->error = "Target not found";
			return false;
		}
		TBAGame->playerChar->setTarget(newTarget);
	}
	return true;
}

//Zoom
std::string zoomFunc(Command* command, const std::vector<std::string> &args) {
	
	if(command->aux == "out") {
		TBAGame->gameWindow->mapScreen->zoom /= 2;
	} else if(command->aux == "in") {
		TBAGame->gameWindow->mapScreen->zoom *= 2;
	} else {
		TBAGame->gameWindow->mapScreen->zoom = 1;
		TBAGame->gameWindow->mapScreen->generateMapTiles();
		return "\nReset zoom";
	}
	TBAGame->gameWindow->mapScreen->generateMapTiles();
	return "\nZoomed "+toLower(command->aux);
}
bool zoomEC(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		command->aux = "";
		return true;
	}
	if(toLower(args.at(0)) != "out" and toLower(args.at(0)) != "in") {
		command->error = "Invalid zoom direction";
		return false;
	}
	command->aux = args.at(0);
	return true;
}

//Say
std::string sayFunc(Command* command, const std::vector<std::string> &args) {
	TBAGame->playerChar->say(join(' ',args));
	return "";
}
bool sayEC(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		command->error = "";
		return false;
	}
	return true;
}

//Hurtme
std::string hurtmeFunc(Command* command, const std::vector<std::string> &args) {
	TBAGame->playerChar->health -= std::stoi(args.at(0));
	return "\nHurt player for "+args.at(0);
}
bool hurtmeEC(Command* command, const std::vector<std::string> &args) {
	return true;
}

//Exit
std::string exitFunc(Command* command, const std::vector<std::string> &args) {
	TBAGame->gameRunning = false;
	return "\nExiting";
}

//Take
std::string takeFunc(Command* command, const std::vector<std::string> &args) {
	
	return parseInteraction(command,args);

}
bool takeEC(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		command->error = "No item specified";
		return false;
	}
	//Check if player has selected a valid container
	if(!TBAGame->hasDisplayTarget()) {
		command->error = "No container selected";
		return false;
	}
	if(TBAGame->displayTarget->type != OBJ_CONTAINER and TBAGame->displayTarget->type != OBJ_CHARACTER) {
		command->error = "Cannot take item from "+TBAGame->displayTarget->getName();
		return false;
	}
	return true;
}

//Select
std::string selectFunc(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		if(TBAGame->hasDisplayTarget()) {
			std::string name = TBAGame->displayTarget->getName();
			TBAGame->clearDisplayTarget();
			return "\nDeselected "+name;
		}
		return "";
	}
	std::string objName = join(' ',args);
	GameObject *newDisplayTarget = TBAGame->playerChar->findObjectInRadius(objName);
	if(newDisplayTarget == nullptr) {
		return "\nObject not found";
	} else {
		TBAGame->setDisplayTarget(newDisplayTarget);
	}
	return "\nSelected "+newDisplayTarget->getName();
}
bool selectEC(Command* command, const std::vector<std::string> &args) {
	return true;
}

//Put
std::string putFunc(Command* command, const std::vector<std::string> &args) {
	return parseInteraction(command,args);
}
bool putEC(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		command->error = "No item specified";
		return false;
	}
	//Check if player has selected a valid container
	if(!TBAGame->hasDisplayTarget()) {
		command->error = "No target selected";
		return false;
	}
	if(!TBAGame->displayTarget->hasInventory()) {
		command->error = "Cannot put item in "+TBAGame->displayTarget->getName();
		return false;
	}
	return true;
}

//Search
std::string searchFunc(Command* command, const std::vector<std::string> &args) {
	if(args.size() > 0) {
		std::string containerName = join(' ',args);
		std::vector<GameObject*> containers = TBAGame->playerChar->getObjectsInRadius(OBJ_CONTAINER);
		for(int i=0;i<containers.size();i++) {
			if(startsWith(toLower(containers.at(i)->getName()),toLower(containerName))) {
				return "\n"+containers.at(i)->getInvString();
			}
		}
		return "\nContainer not found";
	} else {
		return "\n"+TBAGame->displayTarget->getInvString();
	}
}
bool searchEC(Command* command, const std::vector<std::string> &args) {
	//Check if player has selected a valid container
	if(!TBAGame->hasDisplayTarget() and args.size() == 0) {
		command->error = "No target selected";
		return false;
	}
	return true;
}


//Equip
std::string equipFunc(Command* command, const std::vector<std::string> &args) {
	std::string itemName = join(' ',args);
	int index = TBAGame->playerChar->inventory->find(itemName);
	if(index >= 0) {
		Item *goodItem = TBAGame->playerChar->inventory->getItem(index);
		if(TBAGame->playerChar->equip(goodItem)) return "\nEquipped "+goodItem->name;
		return "\nCan't equip this item";
	} else if(index == -2) {
		return "";
	}
	return "\nItem not found";
}
bool equipEC(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		command->error = "No item specified";
		return false;
	}
	return true;
}

//Debug
std::string debugFunc(Command* command, const std::vector<std::string> &args) {
	debugKey();
	return "";
}


//Examine
std::string examineFunc(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		return "\nStanding on "+TBAGame->playerChar->location->getName();
	} else {
		int index = TBAGame->playerChar->inventory->find(join(' ',args));
		if(index >= 0) {
			return "\n"+TBAGame->playerChar->inventory->getItem(index)->getInfo();
		} else if(index == -2) {
			return "";
		} else {
			return "\nItem not found";
		}
	}
}

bool examineEC(Command* command, const std::vector<std::string> &args) {
	return true;
}

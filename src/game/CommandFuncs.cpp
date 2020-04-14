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
std::map<std::string,std::pair<std::string,int>> dirMap = {

	{"north",{"North",270}},
	{"south",{"South",90}},
	{"east",{"East",0}},
	{"west",{"West",180}},
	{"up",{"North",270}},
	{"down",{"South",90}},
	{"right",{"East",0}},
	{"left",{"West",180}},
	{"n",{"North",270}},
	{"s",{"South",90}},
	{"e",{"East",0}},
	{"w",{"West",180}},
	{"northwest",{"Northwest",225}},
	{"southwest",{"Southwest",135}},
	{"northeast",{"Northeast",315}},
	{"southeast",{"Southeast",45}},
	{"nw",{"Northwest",225}},
	{"sw",{"Southwest",135}},
	{"ne",{"Northeast",315}},
	{"se",{"Southeast",45}},

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

	TBAGame->playerChar->targetAng = (float)dirMap.at(args.at(0)).second;
	TBAGame->playerChar->autoMove = true;
	TBAGame->playerChar->addStatus(STATUS_MOVE);
	return "\nMoving "+command->aux;

}
bool moveEC(Command* command, const std::vector<std::string>& args) {

	if(args.size() == 0 or dirMap.find(args.at(0)) == dirMap.end()) {
		command->error = "Invalid direction";
		return false;
	}
	command->aux = dirMap.at(args.at(0)).first;
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
	
	// TBAGame->playerChar->direction = {0,0};
	TBAGame->playerChar->autoMove = false;
	return "\nStopped moving";

}

//Help
std::string helpFunc(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) 
		return "\n=====================\nFor information type:\n->help <command>\n=====================\n"+join('\n',TBAGame->commandStrings);
	std::string cmd = join(' ',args);
	if(helpMap.find(cmd) != helpMap.end()) {
		return "\nUsage: "+join("\n       ",helpMap.at(cmd).first)+"\n\nFunction: "+join("\n          ",helpMap.at(cmd).second);
	}
	return "No help entry for this command";
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
		// if(!TBAGame->playerChar->getNearestTarget()) return "\nNo valid targets found";
		// return "\n"+TBAGame->playerChar->getName()+" targets "+TBAGame->playerChar->getTargetName();
		TBAGame->playerChar->setTarget(nullptr);
		return "\nReset target";
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
	int damage = 0;
	if(args.size() == 0) {
		damage = 0;
	} else {
		damage = std::stoi(args.at(0));
	}
	TBAGame->playerChar->health -= damage;
	return "\nHurt player for "+std::to_string(damage);
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
				TBAGame->displayText("\nSearching "+containers.at(i)->getName()+"...");
				return containers.at(i)->getInvString();
			}
		}
		return "\nContainer not found";
	} else {
		TBAGame->displayText("\nSearching "+TBAGame->displayTarget->getName()+"...");
		return TBAGame->displayTarget->getInvString();
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
		if(TBAGame->playerChar->equip(goodItem)) {
			TBAGame->playerChar->resetCombatTimer();
			return "\nEquipped "+goodItem->name;
		}
		return "\nCan't equip this item ("+goodItem->name+")";
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
		std::string surface = "\nStanding on "+TBAGame->playerChar->location->getName();
		std::vector<Character*> nearbyChars = TBAGame->playerChar->getCharactersInRadius();
		std::vector<GameObject*> nearbyObjs = TBAGame->playerChar->getObjectsInRadius(OBJ_GENERIC);
		if(nearbyChars.size() == 0 and nearbyObjs.size() == 0) return surface;
		surface += "\nNearby:";
		for(int i=0;i<nearbyChars.size();i++) {
			surface += "\n-"+nearbyChars.at(i)->getName();
		}
		for(int i=0;i<nearbyObjs.size();i++) {
			surface += "\n>"+nearbyObjs.at(i)->getName();
		}
		return surface;
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

//Plant
std::string plantFunc(Command* command, const std::vector<std::string> &args) {
	std::string itemName = join(' ',args);
	int index = TBAGame->playerChar->inventory->find(itemName);
	if(index >= 0) {
		Item *goodItem = TBAGame->playerChar->inventory->getItem(index);
		if(TBAGame->playerChar->plant(goodItem)) {
			return "\nPlanted "+goodItem->name;
		}
		return "\nCan't plant this item ("+goodItem->name+")";
	} else if(index == -2) {
		return "";
	}
	return "\nItem not found";
}
bool plantEC(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		command->error = "No item specified";
		return false;
	}
	return true;
}















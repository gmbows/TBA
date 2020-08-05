#include "CommandFuncs.h"
#include "../common/Common.h"
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

void checkHelp() {
	for(int i=0;i<TBAGame->commandList.size();i++) {
		if(helpMap.find(TBAGame->commandList.at(i)->aliases.at(0)) == helpMap.end()) {
			std::cout << "WARNING No help entry for command: "+TBAGame->commandList.at(i)->aliases.at(0) << std::endl;
		}
	}
}

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

std::map<std::string,int> equipmentMap = {

	{"primary",0},
	{"secondary",1},
	{"head",2},
	{"chest",3},
	{"body",3},
	{"leg",4},
	{"legs",4},
	{"arms",5},
	{"feet",6},

};

//Clear
std::string clearFunc(Command* command,const std::vector<std::string>& args) {
	// TBAGame->gameWindow->textScreen->content.clear();
	return "\nCLIENT ONLY";
}

//Inventory
std::string inventoryFunc(Command* command,const std::vector<std::string>& args) {
	return TBAGame->playerChar->getInvString();
}

//Move + EC
std::string moveFunc(Command* command, const std::vector<std::string> &args) {

	TBAGame->playerChar->targetAng = (float)dirMap.at(args.at(0)).second;
	TBAGame->playerChar->autoMove = true;
	TBAGame->playerChar->addStatus(STATUS_TRAVEL);
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
	
	TBAGame->playerChar->autoMove = false;
	if(TBAGame->playerChar->isAlive()) TBAGame->playerChar->setStatus(STATUS_IDLE);
	TBAGame->playerChar->targetPath.clear();
	return "\nStopped all actions";

}

//Help
std::string helpFunc(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) 
		return "\n==========================\nFor more information type:\n->help <command>\n==========================\n"+join('\n',TBAGame->commandStrings);
	std::string cmd = join(' ',args);
	if(helpMap.find(cmd) != helpMap.end()) {
		std::string newline = "\n";
		return newline+TBAGame->colorKey+"bUsage"+TBAGame->colorKey+": "+join(newline+"       ",helpMap.at(cmd).first)+newline+newline+TBAGame->colorKey+"bFunction"+TBAGame->colorKey+": "+join(newline+"          ",helpMap.at(cmd).second);
	}
	return "\nNo help entry for this command";
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
	if(!TBAGame->playerChar->hasTarget()) return "\nNo target";
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
		// TBAGame->gameWindow->mapScreen->zoom /= 2;
	} else if(command->aux == "in") {
		// TBAGame->gameWindow->mapScreen->zoom *= 2;
	} else {
		// TBAGame->gameWindow->mapScreen->zoom = 1;
		// TBAGame->gameWindow->mapScreen->generateMapTiles();
		return "\nCLIENT ONLY";
	}
	// TBAGame->gameWindow->mapScreen->generateMapTiles();
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
	for(int i=0;i<TBAGame->playerChar->body->getLimbs().size();i++) {
		TBAGame->playerChar->body->getLimbs().at(i)->applyDamage(damage);
	}
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
	if(!TBAGame->displayTarget->hasInventory()) {
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
		if(TBAGame->displayTarget->hasInventory()) {
			TBAGame->displayText("\nSearching "+TBAGame->displayTarget->getName()+"...");
			return TBAGame->displayTarget->getInvString();
		} else {
			return "\nCannot search this object";
		}
	}
}
bool searchEC(Command* command, const std::vector<std::string> &args) {
	//Check if player has selected a valid container
	if(!TBAGame->hasDisplayTarget() and args.size() == 0) {
		command->error = "No object selected";
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

//Unequip
std::string unequipFunc(Command* command, const std::vector<std::string> &args) {
	std::string itemName = join(' ',args);
	const std::string itemNameOriginal = itemName;
	// int index = TBAGame->playerChar->inventory->find(itemName);
	std::vector<std::string> itemNames = TBAGame->getItemNames(TBAGame->playerChar->equipment->slots);
	if(autocomplete(itemName,itemNames)) {
		for(int i=0;i<itemNames.size();i++) {
			if(itemNames.at(i) == itemName) {
				TBAGame->playerChar->equipment->slots.at(i) = nullptr;
				return "\nUnequipped "+itemName;
			}
		}
		//Input matched two items
	}
	debug(itemName);
	if(itemNameOriginal != itemName) return "\n";
	return "\nItem not found or not equipped";
}
bool unequipEC(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		command->error = "No item specified";
		return false;
	}
	return true;

}


//Debug
std::string debugFunc(Command* command, const std::vector<std::string> &args) {
	// debugKey();
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
			surface += "\n -"+nearbyChars.at(i)->getName();
		}
		for(int i=0;i<nearbyObjs.size();i++) {
			surface += "\n >"+nearbyObjs.at(i)->getName();
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

//Drink
std::string drinkFunc(Command* command, const std::vector<std::string> &args) {
	std::string itemName = join(' ',args);
	int index = TBAGame->playerChar->inventory->find(itemName);
	if(index >= 0) {
		Item *goodItem = TBAGame->playerChar->inventory->getItem(index);
		if(TBAGame->playerChar->consume(goodItem)) {
			return "\nDrank "+goodItem->name;
		}
		return "\nCan't drink this item ("+goodItem->name+")";
	} else if(index == -2) {
		return "";
	}
	return "\nItem not found";
}
bool drinkEC(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		command->error = "No item specified";
		return false;
	}
	return true;
}

//Use
std::string useFunc(Command* command, const std::vector<std::string> &args) {
	std::string itemName = join(' ',args);
	int index = TBAGame->playerChar->inventory->find(itemName);
	if(index >= 0) {
		Item *goodItem = TBAGame->playerChar->inventory->getItem(index);
		if(TBAGame->playerChar->consume(goodItem)) {
			return "\nUsed "+goodItem->name;
		}
		return "\nCan't use this item ("+goodItem->name+")";
	} else if(index == -2) {
		return "";
	}
	return "\nItem not found";
}
bool useEC(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		command->error = "No item specified";
		return false;
	}
	return true;
}

//Giveme
std::string givemeFunc(Command* command, const std::vector<std::string> &args) {
	int id = std::stoi(args.at(0));
	Item* newItem = new Item(id);
	TBAGame->playerChar->inventory->add(newItem);
	return "\nGave "+TBAGame->playerChar->getName()+" "+newItem->getFormattedName();
}
bool givemeEC(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		command->error = "No item specified";
		return false;
	}
	return true;
}

//Work
std::string workFunc(Command* command, const std::vector<std::string> &args) {
	if(args.size() > 0) {
		std::string nodeName = join(' ',args);
		std::vector<GameObject*> nodes = TBAGame->playerChar->getObjectsInRadius(OBJ_INTERACTIVE);
		if(autocomplete(nodeName,nodes)) {
			for(int i=0;i<nodes.size();i++) {
					if(nodeName == nodes.at(i)->getName()) {
						if(TBAGame->playerChar->work(nodes.at(i))) {
							return "\nWorking "+nodes.at(i)->getFormattedName();
						}
				}
			}
		}
		return "\nResource not found";
	} else {
		if(TBAGame->playerChar->work(TBAGame->displayTarget)) {
			return "\nWorking "+TBAGame->displayTarget->getFormattedName();
		} else {
			return "\nCannot work this object";
		}
	}
}
bool workEC(Command* command, const std::vector<std::string> &args) {
	//Check if player has selected a valid Node
	if(!TBAGame->hasDisplayTarget() and args.size() == 0) {
		command->error = "No resource selected";
		return false;
	}
	return true;
}

//Goto
std::string gotoFunc(Command* command, const std::vector<std::string> &args) {
	std::string c = strip(join("",args));
	std::vector<std::string> coords = split(',',c);
	if(coords.size() < 2) return "\nInvalid coordinates";
	std::string xs = coords.at(0);
	std::string ys = coords.at(1);
	if(!(isdigit(xs) & isdigit(ys))) return "\nInvalid coordinates";
	if(TBAGame->playerChar->generatePathTo(std::stoi(xs),std::stoi(ys))) {
		return "\nTraveling to "+xs+", "+ys;
	}
	return "\nUnable to travel to "+xs+", "+ys;
}
bool gotoEC(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		command->error = "No coordinates specified";
		return false;
	} else {
		if(!contains(join("",args),",")) {
			command->error = "Invalid coordinate format";
			return false;
		}
	}
	return true;
}

//Control
std::string controlFunc(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		if(TBAGame->hasDisplayTarget()) {
			TBAGame->playerChar = TBAGame->displayTarget->getAsCharacter();
			return "\nNow controlling "+TBAGame->playerChar->getFormattedName();
		}
	}
	std::string objName = join(' ',args);
	GameObject *newDisplayTarget = TBAGame->playerChar->findObjectInRadius(objName);
	if(newDisplayTarget == nullptr) {
		return "\nCharacter not found";
	} else {
		TBAGame->playerChar = newDisplayTarget->getAsCharacter();
		TBAGame->displayTarget = newDisplayTarget;
		return "\nNow controlling "+newDisplayTarget->getFormattedName();
	}
}
bool controlEC(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		if(!TBAGame->hasDisplayTarget()) {
			command->error = "No character selected";
			return false;
		}
	}
	return true;
}
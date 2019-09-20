#include "CommandFuncs.h"
#include "../common/Common.h"
#include "Character.h"
#include "Command.h"
#include "FloatingText.h"

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
	return TBAGame->playerChar->inventory->contentString;
}

//Move + EC
std::string moveFunc(Command* command, const std::vector<std::string> &args) {

	TBAGame->playerChar->direction = std::get<1>(dirMap.at(args.at(0)));
	TBAGame->playerChar->setStatus(MOVE);
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
	if(args.size() == 0) {
		TBAGame->playerChar->getNearestTarget();
		return "\nTargeting nearest character";
	}

	TBAGame->playerChar->setTarget(TBAGame->findObject(command->aux));	
	return "\nTargeting "+TBAGame->playerChar->getTargetName();
}
bool targetEC(Command* command, const std::vector<std::string> &args) {
	if(args.size() == 0) {
		command->aux = "None";
		return true;
	}

	std::string targetName = join(' ',args);
	if(TBAGame->findObject(targetName) == nullptr) {
		command->error = "Target not found";
		return false;
	}
	command->aux = targetName;
	return true;
}

//Attack
std::string attackFunc(Command* command, const std::vector<std::string> &args) {
	TBAGame->playerChar->setStatus(COMBAT);
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
		TBAGame->gameWindow->mapScreen->charW /= 2;
		TBAGame->gameWindow->mapScreen->charH /= 2;
	} else if(command->aux == "in") {
		TBAGame->gameWindow->mapScreen->charW *= 2;
		TBAGame->gameWindow->mapScreen->charH *= 2;
	} else {
		TBAGame->gameWindow->mapScreen->charW = 16;
		TBAGame->gameWindow->mapScreen->charH = 16;
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

//Take
std::string takeFunc(Command* command, const std::vector<std::string> &args) {
	return "\nTook Black";
}
bool takeEC(Command* command, const std::vector<std::string> &args) {
	return true;
}

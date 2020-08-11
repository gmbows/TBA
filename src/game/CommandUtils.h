#pragma once
#include <vector>
#include <map>
#include <utility>

#include "GameObject.h"
#include "Command.h"
#include "../tools/Utility.h"

extern std::map<std::string,std::pair<std::vector<std::string>,std::vector<std::string>>> helpMap;

int moveItems(int itemCount,int itemID,Inventory *source, Inventory *destination);

int findItemSource(std::string itemName,Inventory* &inv,std::string &sourceName);

std::string parseInteraction(Command *cmd, std::vector<std::string> args);
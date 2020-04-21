#pragma once

#include "GameObject.h"
#include "Inventory.h"
#include "Item.h"
#include "../common/Tile.h"
#include "../common/Common.h"

class Character;

struct ResourceNode : public GameObject {
	std::string name;
	Inventory *inventory = new Inventory(64);
	
	int displayID;
	
	float x,y;
	Tile *location;
	
	int resourceID;
	int duration;
	float progress;
	bool harvestable = false;
	
	int inline getDisplayID() {return this->displayID;}
	
	std::string inline getName() {return this->name;}
	std::string getInfo();
	
	std::tuple<float,float> getLocation() {return {this->x,this->y};}
	
	bool work() {
		if(this->tick()) {
			this->inventory->add(this->resourceID);
			this->harvestable = false;
			return true;
		}
		return false;
	}
	
	bool tick() {
		if(++this->progress > this->duration) {
			this->harvestable = true;
			this->progress = 0;
		}
		return this->harvestable;
	}
	
	ResourceNode(const std::string &_name,std::tuple<float,float> loc,int itemID,int _did, int time): name(_name), resourceID(itemID), duration(TBAGame->convert(time*1000)), displayID(_did), GameObject(OBJ_INTERACTIVE) {
		this->x = std::get<0>(loc);
		this->y = std::get<1>(loc);
		this->location = TBAGame->gameWorld->getTileAt(this->x,this->y);
		this->location->addObject(this);
		this->progress = 0;
	}
	
	void cleanup() {
			debug("Deleting resource node");
			delete this->inventory;
			TBAGame->removeObject(this);
			delete this;
			debug("Done deleting resource node");
		}
};
#pragma once

#include "GameObject.h"
#include "Inventory.h"
#include "Item.h"
#include "../common/Tile.h"
#include "../common/Common.h"
#include  "LootTable.h"

class Character;

struct ResourceNode : public GameObject {
	std::string name;
	Inventory *inventory = new Inventory(64);
	
	int displayID;
	
	float x,y;
	Tile *location;
	
	LootTable *table;
	int duration;
	float progress;
	int primaryResource;
	bool harvestable = false;
	
	int inline getDisplayID() {return this->displayID;}
	
	std::string inline getName() {return this->name;}
	std::string inline getFormattedName() {return TBAGame->colorKey+"g"+this->name+TBAGame->colorKey;}
	std::string getInfo();
	
	std::tuple<float,float> getLocation() {return {this->x,this->y};}
	
	bool work() {
		if(this->tick()) {
			this->inventory->add(this->table->trial());
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
	
	ResourceNode(const std::string &_name,std::tuple<float,float> loc,lootTable t,int _did, int time): name(_name), duration(TBAGame->convert(time*1000)), displayID(_did), GameObject(OBJ_INTERACTIVE) {
		// std::cout << this->duration << std::endl;
		// std::cout << "NIGGERS" << std::endl;
		this->x = std::get<0>(loc);
		this->y = std::get<1>(loc);
		this->location = TBAGame->gameWorld->getTileAt(this->x,this->y);
		this->location->addObject(this);
		this->table = new LootTable(t);
		this->primaryResource = t.begin()->second.first;
		for(lootTable::iterator it=t.begin();it!=t.end();it++) {
			if(it->first == 100) this->primaryResource = it->second.first;
		}
		this->progress = 0;
	}
	
	void cleanup() {
			debug("Deleting resource node");
			delete this->inventory;
			delete this->table;
			TBAGame->removeObject(this);
			debug("Done deleting resource node");
		}
};
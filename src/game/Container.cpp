#include "Container.h"
#include "GameObject.h"
#include "Inventory.h"

#include "../common/Tile.h"
#include "../common/Common.h"
#include "../tools/Utility.h"
#include <tuple>

Container::Container(const std::string &_name,std::tuple<float,float> location, int size): name(_name), GameObject(OBJ_CONTAINER) {
	this->inventory = new Inventory(size);
	decompose(location,this->x,this->y);
	this->location = TBAGame->gameWorld->getTileAt(this->x,this->y);
	this->location->addObject(this);

}

Container::Container(const std::string &_name,std::tuple<float,float> location, Inventory* _inventory): name(_name), inventory(_inventory), GameObject(OBJ_CONTAINER) {
	decompose(location,this->x,this->y);
	this->location = TBAGame->gameWorld->getTileAt(this->x,this->y);
	this->location->addObject(this);
}

Container::Container(const std::string &_name,std::tuple<float,float> location, int size, const std::vector<int> &itemVec): name(_name), GameObject(OBJ_CONTAINER) {
	this->inventory = new Inventory(size);
	this->inventory->add(itemVec);
	decompose(location,this->x,this->y);
	this->location = TBAGame->gameWorld->getTileAt(this->x,this->y);
	this->location->addObject(this);
}

std::tuple<float,float> Container::getLocation() {
	return {this->x,this->y};
}

std::string Container::getInfo() {
	return "\n\nName:\t"+this->name + "\n" +
				"\tLocation:\t"+std::to_string((int)std::round(this->x)) + "," + std::to_string((int)std::round(this->y)) + "\n" +
				"\tContents:"+this->inventory->contentString;
}
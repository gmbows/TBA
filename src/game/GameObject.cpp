#include "GameObject.h"
#include "../common/Common.h"

#include "Container.h"
#include "Character.h"
#include "Inventory.h"

class Character;
class Container;

GameObject::GameObject(objType obj): type(obj) {
	this->objectID = ++TBAGame->objectTotal;

	switch(this->type) {
		case OBJ_GENERIC:
			debug("DEBUG: CREATING GENERIC OBJECT");
		case OBJ_BLOCK:
		case OBJ_CHARACTER:
		case OBJ_PROJECTILE:
		case OBJ_CONTAINER:
			TBAGame->gameObjects.push_back(this);
			break;
		case UI_FLOATINGTEXT:
			TBAGame->gameUIObjects.push_back(this);
			break;
	}
	
}

Inventory* GameObject::getInventory() {
	switch(this->type) {
		case OBJ_CHARACTER:
			return static_cast<Character*>(this)->inventory;
		case OBJ_CONTAINER:
			return static_cast<Container*>(this)->inventory;
	}
	return nullptr;
}

std::string GameObject::getInvString() {
	if(!this->hasInventory()) return "ERR_NO_INVENTORY";
	return this->getName()+":"+this->getInventory()->toString();
}


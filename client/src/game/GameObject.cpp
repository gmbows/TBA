#include "GameObject.h"
#include "../common/Common.h"

#include "Container.h"
#include "ResourceNode.h"
#include "Character.h"
#include "Inventory.h"

// class Character;
// class Container;

GameObject::GameObject(objType obj): type(obj) {
	this->objectID = ++TBAGame->objectTotal;

	switch(this->type) {
		case OBJ_BLOCK:
			break;
		case OBJ_CHARACTER:
			TBAGame->gameObjects.push_back(this);
			break;
		case OBJ_PROJECTILE:
			TBAGame->gameObjects.push_back(this);
			break;
		case OBJ_CONTAINER:
			break;
		case OBJ_INTERACTIVE:
			break;
		case UI_FLOATINGTEXT:
			TBAGame->gameUIObjects.push_back(this);
			break;
		default:
			debug("Warning: Creating generic object "+std::to_string(this->objectID));
			break;
	}
	
}

ResourceNode* GameObject::getAsResource() {
	return static_cast<ResourceNode*>(this);
}

Character* GameObject::getAsCharacter() {
	return static_cast<Character*>(this);
}

Container* GameObject::getAsContainer() {
	return static_cast<Container*>(this);
}

Inventory* GameObject::getInventory() {
	switch(this->type) {
		case OBJ_CHARACTER:
			return static_cast<Character*>(this)->inventory;
		case OBJ_CONTAINER:
			return static_cast<Container*>(this)->inventory;
		case OBJ_INTERACTIVE:
			return static_cast<ResourceNode*>(this)->inventory;
	}
	return nullptr;
}

std::string GameObject::getInvString() {
	if(!this->hasInventory()) return "ERR_NO_INVENTORY";
	return this->getInventory()->toString();
}


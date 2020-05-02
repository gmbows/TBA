#include "MiscUtils.h"

#include "GameObject.h"
#include "Container.h"
#include "Character.h"
#include "Inventory.h"

Inventory* GameObject::getInventory() {
	switch(this->type) {
		case OBJ_CHARACTER:
			return static_cast<Character*>(this)->inventory;
		case OBJ_CONTAINER:
			return static_cast<Container*>(this)->inventory;
	}
}
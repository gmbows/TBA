#include "GameObject.h"
#include "../common/Common.h"
//Blank file for inclusion purposes

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
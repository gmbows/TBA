#pragma once
#include "../tools/Utility.h"
#include "Inventory.h"

#include <string>
#include <utility>
#include <tuple>

enum objType {
	OBJ_GENERIC,
	OBJ_CHARACTER,
	OBJ_BLOCK,
	OBJ_PROJECTILE,
	OBJ_CONTAINER,
	UI_FLOATINGTEXT
};

class GameObject {

	public:

		int objectID;
		objType type = OBJ_GENERIC;

		//Update function to be overridden for each object type
		virtual void update() {}
		virtual void cleanup(){}

		virtual std::string getName() {return "NAME_FUNC_UNDEFINED";}
		virtual std::string getInfo() {debug("ERROR");return "INFO_FUNC_UNDEFINED";}
		virtual std::tuple<float,float> getLocation() {return {-1,-1};}
		virtual int getDisplayID() {return -1;}

		inline bool hasInventory() { return this->type == OBJ_CHARACTER or this->type == OBJ_CONTAINER;}
		Inventory* getInventory();
		std::string getInvString();

		GameObject(objType);

		~GameObject() {}

};
#pragma once

#include "ItemManifest.h"
#include "StatusEffect.h"

#include "../tools/Utility.h"

#include <vector>
#include <string>
#include <ctime>
#include <map>

void checkItemTypes();
bool importItems();

namespace ItemUtils {
	bool hasType(int id,ItemType type);
}

class Item {

	public:
	
		//item index in manifest vector
		int id;

		int UUID;

		std::string name;
		std::string description;
		int weight;
		int size;
		time_t created;

		flag types;

		std::string getTypeAsString();
		std::map<ItemAttribute,float> attributes;
		std::map<Action,std::vector<std::vector<float>>> effects;
		
		std::string getInfo();

		Item(int);

		inline flag getType() {return this->types;}
		ItemType getWeaponType();
		inline bool hasType(ItemType type) { return (this->types & type) == type;}

		std::vector<std::string> getTypes();

		bool hasAttribute(ItemAttribute);
		bool hasEffectOnAction(Action);
		float getAttribute(ItemAttribute);
		std::vector<StatusEffect*> getEffectsOnAction(Action);
		void createAttributeSet(const std::vector<ItemAttribute>&,const std::vector<float>&);

		std::string getName();
		std::string getFormattedName();
		std::string getPlural();
		std::string getPluralDisplayName();
		std::string getDisplayName();

		~Item() {
			// std::cout << "Deleting item" << std::endl;
		}

};

inline bool operator==(Item i1,Item i2) {
	return (char*)&i1 == (char*)&i2;
}
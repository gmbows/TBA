#pragma once

#include "ItemManifest.h"
#include "StatusEffect.h"

#include <vector>
#include <string>
#include <ctime>
#include <map>

class Item {

	public:
	
		//item index in manifest vector
		int id;
		
		std::string name;
		std::string description;
		int weight;
		int size;
		time_t created;

		flag types;
		itemType primaryType;
		std::string getTypeAsString();
		std::map<itemAttribute,float> attributes;
		std::vector<StatusEffect*> effects;
		
		std::string getInfo();

		Item(int);

		inline flag getType() {return this->types;}
		inline bool hasType(itemType type) { return (this->types & type) > 0;}

		itemType getPrimaryType();

		bool hasAttribute(itemAttribute);
		bool hasEffect(EffectType);
		std::vector<StatusEffect*> lookupEffects();
		float getAttribute(itemAttribute);
		std::vector<itemAttribute> getAttributes();
		std::vector<StatusEffect*> inline getEffects() { return this->effects; }
		void createAttributeSet(const std::vector<itemAttribute>&,const std::vector<float>&);

		std::string getName();
		std::string getFormattedName();
		std::string getPlural();
		std::string getPluralDisplayName();
		std::string getDisplayName();

		~Item() {
			//delete this->attributes;
		}

};

inline bool operator==(Item i1,Item i2) {
	return (char*)&i1 == (char*)&i2;
}
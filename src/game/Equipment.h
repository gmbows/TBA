#pragma once

#include "Item.h"
#include "ItemManifest.h"

#include <string>

struct Equipment {

	Item* head = nullptr;
	Item* body = nullptr;
	Item* legs = nullptr;
	Item* feet = nullptr;
	
	Item* primary = nullptr;
	Item* secondary = nullptr;

	std::string getEquipString();

	bool inline hasPrimary() {return this->primary != nullptr;}
	bool inline hasSecondary() {return this->primary != nullptr;}
	bool inline hasHead() {return this->head != nullptr;}
	bool inline hasBody() {return this->body != nullptr;}
	bool inline hasLegs() {return this->legs != nullptr;}
	bool inline hasFeet() {return this->feet != nullptr;}

	std::string getPrimaryName() {
		if(this->hasPrimary()) return this->primary->name;
		return "None";
	}
	std::string getSecondaryName() {
		if(this->hasSecondary()) return this->secondary->name;
		return "None";
	}
	std::string getHeadName() {
		if(this->hasHead()) return this->head->name;
		return "None";
	}
	std::string getBodyName() {
		if(this->hasBody()) return this->body->name;
		return "None";
	}
	std::string getLegsName() {
		if(this->hasLegs()) return this->legs->name;
		return "None";
	}
	std::string getFeetName() {
		if(this->hasFeet()) return this->feet->name;
		return "None";
	}

};
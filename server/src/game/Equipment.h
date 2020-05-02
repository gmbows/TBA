#pragma once

#include "Item.h"
#include "ItemManifest.h"
#include "../tools/Utility.h"

#include <string>

enum EquipmentSlot: int {
	EQUIP_PRIMARY,
	EQUIP_SECONDARY,
	EQUIP_HEAD,
	EQUIP_BODY,
	EQUIP_LEGS,
	EQUIP_FEET,
};

struct Equipment {

	std::vector<Item*> slots;
	int numSlots = 6;
	
	std::string getEquipString();

	bool hasEquipped(EquipmentSlot slot) {
		if((int)slot >= this->slots.size()) return false;
		return this->slots.at((int)slot) != nullptr;
	}
	
	bool equip(Item* item,EquipmentSlot slot) {
		if((int)slot >= this->slots.size()) return false;
		this->slots.at((int)slot) = item;
		return true;
	}

	bool unequip(EquipmentSlot slot) {
		this->slots.at((int)slot) = nullptr;
		return true;
	}
	
	Item* getSlot(EquipmentSlot slot) {
		if((int)slot >= this->slots.size()) return nullptr;
		return this->slots.at((int)slot);
	}
	
	std::string getSlotName(EquipmentSlot slot) {
		if((int)slot >= this->slots.size()) return "ERROR Slot inaccessible";
		if(!this->hasEquipped(slot)) return "None";
		return this->slots.at((int)slot)->getName();
	}

	float calcArmor();
	float getDef(EquipmentSlot slot);
	
	Equipment() {
		for(int i=0;i<numSlots;i++) {
			this->slots.push_back(nullptr);
		}
	}

};
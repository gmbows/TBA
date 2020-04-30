#include "Equipment.h"

std::string Equipment::getEquipString() {
	return
	"\n\tPrimary:   " + this->getSlotName(EQUIP_PRIMARY)+
	"\n\tHead:      " + this->getSlotName(EQUIP_HEAD)+
	"\n\tBody:      " + this->getSlotName(EQUIP_BODY)+
	// "\n\tArms:      " + this->getSlotName(EQUIP_ARMS)+
	"\n\tLegs:      " + this->getSlotName(EQUIP_LEGS)+ 
	"\n\tFeet:      " + this->getSlotName(EQUIP_FEET);
}

float Equipment::calcArmor() {
	float def = 0;
	for(int i=0;i<this->slots.size();i++) {
		if(this->slots.at(i) == nullptr) continue;
		if(this->slots.at(i)->hasType(I_ARMOR)) def += this->slots.at(i)->getAttribute(ATTRIB_DEFENSE);
	}
	return def;
}

float Equipment::getDef(EquipmentSlot slot) {
	if((int)slot >= this->slots.size()) return 0;
	if(!this->hasEquipped(slot)) return 0;
	if(!this->getSlot(slot)->hasType(I_ARMOR)) return 0;
	return this->getSlot(slot)->getAttribute(ATTRIB_DEFENSE);
}
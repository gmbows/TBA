#include "Equipment.h"

std::string Equipment::getEquipString() {
	return
	" Primary:\t" + this->getPrimaryName() +
	"\n Head:\t" + this->getHeadName() +
	"\n Body:\t" + this->getBodyName() +
	"\n Legs:\t" + this->getLegsName() + 
	"\n Feet:\t" + this->getFeetName();
}
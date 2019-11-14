#include "Equipment.h"

std::string Equipment::getEquipString() {
	return
	" Primary:   " + this->getPrimaryName() +
	"\n Head:      " + this->getHeadName() +
	"\n Body:      " + this->getBodyName() +
	"\n Legs:      " + this->getLegsName() + 
	"\n Feet:      " + this->getFeetName();
}
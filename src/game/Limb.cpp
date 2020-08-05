#include "Limb.h"
#include <map>

#include "../tools/Utility.h"

std::map<LimbType,std::string> limbMap = {
	{LIMB_HEAD,"Head"},
	{LIMB_TORSO,"Body"},
	{LIMB_ARMS,"Arms"},
	{LIMB_LEGS,"Legs"},
};


std::string Body::getInfo() {
	return
	"\tHead: "+std::to_string(this->getLimb(LIMB_HEAD)->getHealth()) + "/" + std::to_string(this->getLimb(LIMB_HEAD)->maxHealth) + "\n"+
	"\tBody: "+std::to_string(this->getLimb(LIMB_TORSO)->getHealth()) + "/" + std::to_string(this->getLimb(LIMB_TORSO)->maxHealth) + "\n"+
	"\tArms: "+std::to_string(this->getLimb(LIMB_ARMS)->getHealth()) + "/" + std::to_string(this->getLimb(LIMB_ARMS)->maxHealth) + "\n"+
	"\tLegs: "+std::to_string(this->getLimb(LIMB_LEGS)->getHealth()) + "/" + std::to_string(this->getLimb(LIMB_LEGS)->maxHealth) + "\n";
}

float Body::getVitality() {
	float maxhp,curhp;
	for(int i=0;i<this->limbs.size();i++) {
		maxhp += this->limbs.at(i)->getMaxHealth();
		curhp += this->limbs.at(i)->getHealth();
	}
	return curhp/maxhp;
}

float Body::getVitality(LimbType type) {
	return this->getLimb(type)->getHealth()/this->getLimb(type)->getMaxHealth();
}

std::string Body::toString(LimbType type) {
		if(limbMap.find(type) == limbMap.end()) return "ERR_LIMB_INVALID";
		return limbMap.at(type);
	}
	
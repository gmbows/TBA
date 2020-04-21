#include "StatusEffect.h"
#include "../common/Common.h"
#include "Character.h"
#include "Limb.h"

std::map<EffectType,std::string> effectTypeMap = {
	{EFFECT_END,"ERR_EFFECT_UNASSIGNED"},
	{EFFECT_HEALING,"Regeneration"},
	{EFFECT_DAMAGE,"Poison"},
	{EFFECT_BURN,"Burning"},
};

std::map<ApplicationType,std::string> applicationTypeMap = {
	{APP_INSTANT,"Instant"},
	{APP_OT,"Over time"},
	{APP_PERIODIC,"Periodic"},
};


std::string StatusEffect::getTypeString() {
	if(effectTypeMap.find(this->type) == effectTypeMap.end()) return "ERR_NO_EFF_TYPE_STRING";
	return effectTypeMap.at(this->type);
}

std::string StatusEffect::getDurationString() {
	float perc = (this->remaining*(1000/TBAGame->logicTickRate));
	perc /= 1000;
	return std::to_string((int)perc+1)+"s";
}

// void StatusEffect::determineEffect(Limb *limb,void (Limb::*func)(float)) {
	// if(this->duration==0) {
		// (limb->*func)((float)this->magnitude);
	// } else if(this->period == 0) {
		// if((this->duration-this->remaining)%this->period != 0) return;
		// (limb->*func)((float)this->period*(this->magnitude/this->duration));
	// } else {
		// (limb->*func)((float)this->magnitude/this->duration);
	// }
// }

void StatusEffect::determineEffect(Character *target,void (Limb::*func)(float)) {
	for(int i=0;i<target->limbs.size();i++) {
		if(this->duration==0) {
			(target->limbs.at(i)->*func)((float)this->magnitude);
		} else if(this->period == 0) {
			if((this->duration-this->remaining)%this->period != 0) return;
			(target->limbs.at(i)->*func)((float)this->period*(this->magnitude/this->duration));
		} else {
			(target->limbs.at(i)->*func)((float)this->magnitude/this->duration);
		}
	}
}

bool StatusEffect::applyEffect(Character *target) {
	//healing
	//poison
	//burn
	switch(this->type) {
		case EFFECT_HEALING: {
			// for(int i=0;i<target->limbs.size();i++) {
				this->determineEffect(target,target->limbs.at(0)->applyHealing);
			// }
			break;
		}
		case EFFECT_DAMAGE: {
			// for(int i=0;i<target->limbs.size();i++) {
				this->determineEffect(target,target->limbs.at(0)->applyDamage);
			// }
				break;
		}
		default:
			debug("Applying unassigned effect "+this->getTypeString()+" to "+target->getName());
			break;
	}
	return this->tick();
}
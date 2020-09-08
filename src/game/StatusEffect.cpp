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

// std::map<ApplicationType,std::string> applicationTypeMap = {
	// {APP_INSTANT,"Instant"},
	// {APP_OT,"Over time"},
	// {APP_PERIODIC,"Periodic"},
// };


std::string StatusEffect::getTypeString() {
	if(effectTypeMap.find(this->type) == effectTypeMap.end()) return "ERR_NO_EFF_TYPE_STRING";
	return effectTypeMap.at(this->type);
}

std::string StatusEffect::getDurationString() {
	float perc = (this->remaining*(1000/TBAGame->logicTickRate));
	perc /= 1000;
	return std::to_string((int)perc+1)+"s";
}

void StatusEffect::determineEffect(Character *target,void (Limb::*func)(float)) {
	for(int i=0;i<target->body->getLimbs().size();i++) {
		if(this->duration==0) {
			//Instant
			(target->body->getLimbs().at(i)->*func)((float)this->magnitude);
		} else if(this->period > 0) {
			//Periodic
			if(this->remaining == this->duration) continue;
			if((this->duration-this->remaining)%this->period != 0) return;
			float amt = (float)(this->magnitude);
			(target->body->getLimbs().at(i)->*func)(amt);
		} else {
			//Constant over duration
			(target->body->getLimbs().at(i)->*func)((float)this->magnitude/this->duration);
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
				this->determineEffect(target,target->body->getLimbs().at(0)->applyHealing);
			// }
			break;
		}
		case EFFECT_DAMAGE: {
			// for(int i=0;i<target->limbs.size();i++) {
				this->determineEffect(target,target->body->getLimbs().at(0)->applyDamage);
			// }
				break;
		}
		case EFFECT_BURN: {
			// for(int i=0;i<target->limbs.size();i++) {
				this->determineEffect(target,target->body->getLimbs().at(0)->applyDamage);
			// }
				break;
		}
		default:
			debug("Applying unassigned effect "+this->getTypeString()+" to "+target->getName());
			break;
	}
	return this->tick();
}
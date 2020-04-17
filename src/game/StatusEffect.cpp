#include "StatusEffect.h"
#include "../common/Common.h"

std::map<EffectType,std::string> effectTypeMap = {
	{EFFECT_HEALING,"Regeneration"},
	{EFFECT_DAMAGE,"Poison"},
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
#pragma once
#include <map>

enum EffectType {
	EFFECT_HEALING,
	EFFECT_DAMAGE,
};

struct StatusEffect {
	bool active;

	EffectType type;
	int duration;
	int remaining;
	int magnitude;
	
	std::string getTypeString();
	std::string getDurationString();
	
	void tick() {
		if(--this->remaining < 0) {
			this->active = false;
		}
	}
	
	StatusEffect(EffectType _type, int dur,int mag): type(_type), duration(dur), magnitude(mag) {
		this->active = true;
		this->remaining = duration;
	}
	~StatusEffect();
		
};
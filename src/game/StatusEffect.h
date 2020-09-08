#pragma once
#include <map>
#include <iostream>

class Character;
class Limb;

// enum ApplicationType {
	// APP_INSTANT,		//Ignores duration, applies magnitude once
	// APP_OT,				//Applies magnitude spread evenly over duration
	// APP_PERIODIC,	//Applies magnitude spread periodically over (duration) applications
// };

enum EffectType {
	EFFECT_HEALING,
	EFFECT_DAMAGE,
	EFFECT_BURN,
	EFFECT_END,
};

struct StatusEffect {
	bool active;

	EffectType type;
	int duration;
	int remaining;
	int period;
	float magnitude;
	
	std::string getTypeString();
	std::string getDurationString();
	
	bool applyEffect(Character*);
	void determineEffect(Character*,void (Limb::*)(float));
	
	bool tick() {
		if(--this->remaining < 0) {
			this->active = false;
		}
		return this->active;
	}
	
	StatusEffect(EffectType _type, float mag,int dur,int per=0): type(_type), magnitude(mag), duration(dur), period(per)  {
		this->active = true;
		this->remaining = duration;
	}
	~StatusEffect() {
		std::cout << "Deleting status effect" << std::endl;
		delete this;
	}
		
};
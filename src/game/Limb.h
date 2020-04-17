#pragma once
#include <iostream>

enum limbType {	
							LIMB_HEAD,
							LIMB_ARMS, 
							LIMB_TORSO,
							LIMB_LEGS,
};

struct Limb {

	float health;
	int maxHealth;

	limbType type;
	
	Limb(int max,limbType _type): maxHealth(max), type(_type) {
		this->health = max;
	}
	
	inline int getHealth() {return this->health;}
	inline void applyDamage(float damage) {this->health -= damage;}
	inline void applyHealing(float amount) {this->health += amount;}

};
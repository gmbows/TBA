#pragma once

enum limbType {	
							LIMB_HEAD,
							LIMB_ARMS, 
							LIMB_TORSO,
							LIMB_LEGS,
};

struct Limb {

	int health;
	int maxHealth;

	limbType type;
	
	Limb(int max,limbType _type): maxHealth(max), type(_type) {
		this->health = max;
	}
	
	inline int getHealth() {return this->health;}
	inline void applyDamage(int damage) {this->health -= damage;}

};
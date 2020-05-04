#pragma once
#include <iostream>
#include <vector>
#include <map>

enum LimbType {	
	LIMB_HEAD,
	LIMB_TORSO,
	LIMB_ARMS, 
	LIMB_LEGS,
};

struct Limb {

	float health;
	int maxHealth;

	LimbType type;
	
	std::string serialize();
	
	Limb(int max,LimbType _type): maxHealth(max), type(_type) {
		this->health = max;
	}
	
	inline int getHealth() {return this->health;}
	inline int getMaxHealth() {return this->maxHealth;}
	inline void applyDamage(float damage) {this->health -= std::max(0.0f,damage);}
	inline void applyHealing(float amount) {this->health += std::max(0.0f,amount);}

};

struct Body {
	
	int numLimbs = 4;
	
	std::vector<Limb*> limbs;
	
	std::string serialize();
	
	float getVitality();
	float getVitality(LimbType);
	
	Limb* getLimb(LimbType limb) {
		return this->limbs.at((int)limb);
	}

	std::string toString(LimbType type);

	std::vector<Limb*> inline getLimbs() { return this->limbs;}
	
	std::string getInfo();
	
	Body() {
		for(int i=0;i<this->numLimbs;i++) {
			this->limbs.push_back(new Limb(100,(LimbType)i));
		}
	}
};
#include "Statistics.h"
#include  "../common/Common.h"
	
void Character::init_stats() {

	//Rates in format ms per action
	this->defaultAttackRate = 100;

	//Unarmed damage
	this->defaultAttackDamage = 3;

	//Unarmed striking speed
	this->defaultAttackSpeed = 20;

	this->maxHealth = 10;
	this->health = this->maxHealth;

	//In format 
	this->defaultAttackRange = 1*.5;
	this->defaultMoveSpeed = 2;
	
	this->defaultTurnSpeed = 4;
	this->turnSpeed = this->defaultTurnSpeed;
	this->defaultFOV = 90;

	this->attackRate = this->defaultAttackRate + ((-500+rand()%1000)/50);

}
#include "Character.h"
#include "../common/Common.h"
#include "GameObject.h"
#include "../tools/StringFuncs.h"
#include "FloatingText.h"
#include "Projectile.h"
#include <tuple>

std::map<statusIndicator,const std::string> statusMap = {
	{IDLE,"Idle"},
	{ATTACK,"Swinging weapon"},
	{COMBAT,"In combat"},
	{PURSUE,"Pursuing target"},
	{MOVE,"Traveling"},
	{ESCAPE,"Escaping"},
	{PATROL,"Patrolling"},
	{STUN,"Stunned"},
	{CRIPPLED,"Crippled"},
	{DYING,"Dying"},
	{DEAD,"Dead"},
};

//==========
//	TARGETING
//==========

std::string Character::getTargetInfo() {

	if(!this->hasTarget()) {return "None";}
	return this->getCharTarget()->getInfo();
	
}

std::string Character::getTargetName() {

	if(this->target == nullptr) {return "None";}

	return this->target->getName();
}

void Character::setTarget(GameObject *o) {
	this->target = o;
}

Character* Character::getCharTarget()  {
	return static_cast<Character*>(this->target);
}

GameObject* Character::getTarget()  {
	return this->target;
}


void Character::setStatus(statusIndicator newStatus) {
	if(this->getStatus() == DEAD) {
		return;
	}
	if(newStatus == IDLE) {
		this->direction = {0,0};
	} else if(newStatus == COMBAT and this->status != COMBAT) {
		// Character must be stationary to enter combat
		this->direction = {0,0};
		this->resetCombatTimer();// - (this->attackRate/2);
	}
	this->status = newStatus;
}

statusIndicator Character::getStatus()  {
	return this->status;
}

bool Character::targetInRange() {
	if(!this->hasTarget()) {
		return false;
	}
	//If using a ranged weapon range is irrelevant
	if(this->getAttackRange() < 0) {
		return true;
	}
	return dist(this->getLocation(),this->getCharTarget()->getLocation()) <= this->getAttackRange();
}

bool Character::getNearestTarget() {

	//Target highest priority target in awareness range (placeholder 10)
	std::vector<Tile*> surroundingTiles = TBAGame->gameWorld->getTilesInRadius(this->x,this->y,10);
	Tile* thisTile;

	Character* occupant;
	Character* bestTarget = nullptr;

	for(int i=0;i<surroundingTiles.size();i++) {
		thisTile = surroundingTiles.at(i);
		if(thisTile->isOccupied()) {
			for(int j=0;j<thisTile->occupiers.size();j++) {
	
				occupant = thisTile->occupiers.at(j);

				// Valid target conditions: 
				//	Target is alive
				//	Target is targeting you 
				// Target is attacking, in combat with, or in pursuit of you

				if(occupant->isAlive() and !occupant->isPlayer) {
					if(bestTarget == nullptr) {
						bestTarget = occupant;
						continue;
					}
					if(dist(this->getLocation(),occupant->getLocation()) < dist(this->getLocation(),bestTarget->getLocation())) {
						bestTarget = occupant;
					}	
				}
			}
		}
	}
	if(bestTarget->isPlayer) {
		return false;
	} else {
		this->setTarget(bestTarget);
		return true;
	}

}

bool Character::combatRetarget() {

	//Target highest priority target in awareness range (placeholder 10)
	std::vector<Tile*> surroundingTiles = TBAGame->gameWorld->getTilesInRadius(this->x,this->y,10);
	Tile* thisTile;

	Character* occupant;

	for(int i=0;i<surroundingTiles.size();i++) {
		thisTile = surroundingTiles.at(i);
		if(thisTile->isOccupied()) {
			for(int j=0;j<thisTile->occupiers.size();j++) {
	
				occupant = thisTile->occupiers.at(j);

				// Valid target conditions: 
				//	Target is alive
				//	Target is targeting you 
				// Target is attacking, in combat with, or in pursuit of you

				if(occupant->isAlive() and
					occupant->hasTarget() and
					(char*)occupant->getCharTarget() == (char*)this and
					(occupant->getStatus() == COMBAT or occupant->getStatus() == PURSUE or occupant->getStatus() == ATTACK)) {
						if(!this->hasTarget()) {
							this->setTarget(occupant);
						} else {
							if(dist(this->getLocation(),occupant->getLocation()) < dist(this->getLocation(),this->getCharTarget()->getLocation())) {
								this->setTarget(occupant);
							}
						}
				}
			}
		}
	}
	//ALWAYS RETURNS FALSE!!
	return false;
}

void Character::moveTo(std::tuple<float,float> location) {
	int x = (this->x > this->getCharTarget()->x)? -1 : 1;
	int y = (this->y > this->getCharTarget()->y)? 1 : -1;
	this->direction = {x,y};
}

//==========
//		MOVE
//==========

void Character::setLocomotion() {

	if(!this->isPlayer) {
		if(this->hasTarget()) {
			this->moveTo({this->getCharTarget()->x,this->getCharTarget()->y});
		}
	}

	this->velocityX = std::max(-this->maxMoveSpeed,std::min(this->maxMoveSpeed,this->velocityX+std::get<0>(this->direction)));
	this->velocityY = std::max(-this->maxMoveSpeed,std::min(this->maxMoveSpeed,this->velocityY+std::get<1>(this->direction)));

	if(this->velocityX == 0 and this->velocityY == 0) {
		this->setStatus(IDLE);
		return;
	}


}

//==========
//	  COMBAT
//==========

void Character::combat() {

	//====================
	//		ACQUIRE TARGET
	//====================

	if(this->getCharTarget() == nullptr) {
		this->combatRetarget();
		this->setStatus(IDLE);
		return;
	}

	//============================
	//		PREPARE ATTACK CONDITIONS
	//============================

	if(!this->targetInRange()) {
		if(this->isPlayer) {
			//If target is out of range, treat as if character is not in combat
			this->resetCombatTimer();
			
			//TBAGame->displayText("\nTarget out of range");
			TBAGame->popupText(.5,"Distance from target: "+std::to_string(2*dist(this->getLocation(),this->getCharTarget()->getLocation()))+"m");
		} else {
			// If NPC's target is out of its range, move towards it 
			this->setStatus(PURSUE);
		}
		return;
	}

	if(!this->getCharTarget()->isAlive()) {
		if(this->isPlayer) {
			//Placeholder for behavior when in combat with dead target
		} else {
			//Placeholder for behavior when in combat with dead target
		}
		return;
	}
	//====================
	//		PREPARE ATTACK
	//====================
	
	//Stop moving to prepare attack
	this->direction = {0,0};

	//Failsafe: If character enters preparation phase while attack is ready, reset attack timer to weapon swing time

	switch(this->getAttackStatus()) {
		//Character is in combat but not ready to attack
		case ATK_NOT_READY:
			this->setStatus(COMBAT);
			return;
		//Character is currently attacking with weapon
		//Wait for attack to connect
		case ATK_ATTACKING:
			this->setStatus(ATTACK);
			return;
		//Swing makes contact, send attack and calculate damage
		case ATK_COMPLETE:
			break;
	}


	//====================
	//		SEND ATTACK
	//====================

	this->sendAttack(this->target);

}

void Character::resetCombatTimer() {
	this->lastAttack = SDL_GetTicks();
}

attackStatus Character::getAttackStatus() {
	
	if(SDL_GetTicks() >= this->lastAttack + this->attackRate + this->defaultAttackSpeed) {
		return ATK_COMPLETE;
	} else if(SDL_GetTicks() >= this->lastAttack + this->attackRate) {
		return ATK_ATTACKING;
	}
	return ATK_NOT_READY;//(SDL_GetTicks() >= this->lastAttack + this->attackRate);
}

itemType Character::getAttackType() {
	if(this->equipment->primary == nullptr) {
		return MELEE;
	}
	if(this->equipment->primary->hasType(MELEE)) return MELEE;
	return RANGED;
}

void Character::sendAttack(GameObject *target) {

	//Sends attack type and specifics to target
	//Returns actual damage dealt

	// Retrieve character's current damage output
	int damage = this->getAttackDamage();

	switch(this->getAttackType()) {
		case RANGED:
			float x,y;
			decompose(this->getLocation(),x,y);
			float tx,ty;
			decompose(this->target->getLocation(),tx,ty);

			// DEBUG:: Replace rand range with accuracy deviation and projectile speed
			new Projectile(this,this->getLocation(),(-15+rand()%31)-atan2(ty-y,tx-x)/(3.1415/180),.02);
			break;
		case MELEE:
			// Send to target to be changed based on damage resistance
			static_cast<Character*>(target)->receiveAttack(damage,this);
			break;
	}

	// At this point the attack is complete
	this->lastAttack = SDL_GetTicks();

}

void Character::receiveAttack(int damage,GameObject *attacker) {

	//Takes attack type and specifics
	//Performs resistance calculation, returns damage dealt

	switch(this->getStatus()) {
		case COMBAT:
		case PURSUE:
		case ATTACK:
			break;
		default:
			this->setStatus(COMBAT);
	}

	// Swap target to attacker if attacker is a higher priority than current target
	// Swap conditions: 
	//	You have no target
	//	Target is dead
	//	Target is not targeting you
	//	Target is out of range
	if(this->getCharTarget() == nullptr or !this->targetInRange() or !this->getCharTarget()->isAlive() or (char*)this->getCharTarget()->getCharTarget() != (char*)this) {
		this->setTarget(static_cast<Character*>(attacker));
	}

	//Apply damage resistance calculation
	this->health -= damage;

	// Apply knockback
	int magnitude = damage;
	float directionX = -(this->getCharTarget()->x-this->x);
	float directionY = this->getCharTarget()->y-this->y;
	this->move({magnitude*directionX,magnitude*directionY});

	//Damage popups
	if(this->isPlayer) {
		new FloatingText(3,{std::to_string(damage)},{this->location->x,-this->location->y},{160,20,20,255});
		TBAGame->displayText("\nReceived "+std::to_string(damage)+" damage from "+attacker->getName());
	} else {
		new FloatingText(3,{std::to_string(damage)},{this->location->x,-this->location->y},{20,160,20,255});
		//new FloatingText(3,{"Blessings of King upon you"},this->getAbsoluteLocation(),this);
	}

	if(static_cast<Character*>(attacker)->isPlayer) {
		TBAGame->displayText("\nDealt "+std::to_string(damage)+" damage to "+this->getName());
	}

	//====================
	//	  REACT POST ATTACK
	//====================

	if(this->health <= 0) {
		if(static_cast<Character*>(attacker)->isPlayer) TBAGame->displayText("\nKilled "+this->getName());
		this->kill();

		static_cast<Character*>(attacker)->target = nullptr;
		static_cast<Character*>(attacker)->say("It's a shame, really");

		//If target is killled
		//Decide whether to disengage or choose new target
		//
		//Should check if targets in awareness range are targeting and attacking

		if(!static_cast<Character*>(attacker)->combatRetarget()) {
			static_cast<Character*>(attacker)->setStatus(IDLE);
		}

	}

}

void Character::say(const std::string& message) {
	new FloatingText(3,TBAGame->gameWindow->textScreen->prepareCommandForDisplay(message),this->getApproximateLocation(),this);
}

//==========
//	  UPDATE
//==========

void Character::update() {

	if(!this->isAlive() and this->getStatus() != DEAD) {
		this->kill();
	}
	switch(this->getStatus()) {
		case DEAD:
			break;
		case IDLE:
			//this->decideOnNewAction()
			break;
		case COMBAT:
		case ATTACK:
			this->combat();
			break;
		case PURSUE:
			if(this->targetInRange()) {
				this->setStatus(COMBAT);
			}
			this->setLocomotion();
			break;
		case MOVE:
			this->setLocomotion();
			break;
		case ESCAPE:
			break;
	}

	//"Physics"
	this->move(this->direction);

}

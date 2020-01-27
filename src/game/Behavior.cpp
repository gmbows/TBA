#include "Character.h"
#include "../common/Common.h"
#include "GameObject.h"
#include "../tools/Utility.h"
#include "FloatingText.h"
#include "Projectile.h"
#include <tuple>

std::map<statusIndicator,const std::string> statusMap = {
	{STATUS_IDLE,"Idle"},
	{STATUS_ATTACK,"Swinging weapon"},
	{STATUS_COMBAT,"In combat"},
	{STATUS_PURSUE,"Pursuing target"},
	{STATUS_MOVE,"Traveling"},
	{STATUS_ESCAPE,"Escaping"},
	{STATUS_PATROL,"Patrolling"},
	{STATUS_STUN,"Stunned"},
	{STATUS_CRIPPLED,"Crippled"},
	{STATUS_DYING,"Dying"},
	{STATUS_DEAD,"Dead"},
};

//==========
//	TARGETING
//==========

std::string Character::getTargetInfo() {

	if(!this->hasTarget()) {return "None";}
	return this->getCharTarget()->getInfo();
	
}

std::string Character::getTargetName() {

	if(!this->hasTarget()) {return "None";}

	return this->target->getName();
}

void Character::setTarget(GameObject *o) {
	this->resetCombatTimer();
	this->target = o;
	if(o != nullptr and !this->isPlayer) {
		this->setTargetAngle(this->getCharTarget());
	}	
}

Character* Character::getCharTarget()  {
	return static_cast<Character*>(this->target);
}

GameObject* Character::getTarget()  {
	return this->target;
}

void Character::addStatus(statusIndicator newStatus) {
	if(newStatus == STATUS_COMBAT and !this->hasStatus(STATUS_COMBAT)) {
		// Character must be stationary to enter combat
		this->removeStatus(STATUS_MOVE);
		this->resetCombatTimer();
	}
	if(newStatus != STATUS_IDLE) this->removeStatus(STATUS_IDLE);
	this->status = (this->status | newStatus);
}

void Character::setStatus(statusIndicator newStatus) {
	//remove all other statuses and replace with newStatus
	// really only used for death
	if(newStatus == STATUS_IDLE) {
		// this->direction = {0,0};
	} else if(newStatus == STATUS_COMBAT and !this->hasStatus(STATUS_COMBAT)) {
		// Character must be stationary to enter combat
		// this->direction = {0,0};
		this->resetCombatTimer();// - (this->attackRate/2);
	}
	this->status = newStatus;
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
	if(bestTarget == nullptr or (char*)bestTarget == (char*)this) {
		return false;
	} else {
		this->setTarget(bestTarget);
		return true;
	}

}

bool Character::findTargetInRadius(const std::string& _name) {
	//Find a target with provided partial or complete name
	//If multiple matching targets are found cycle through them
	std::vector<Tile*> tileSet = TBAGame->gameWorld->getTilesInRadius(this->x,this->y,10); //placeholder until awareness stat
	GameObject* testObj;
	for(int i=0;i<tileSet.size();i++) {
		if(tileSet.at(i)->isOccupied()) {
			for(int j=0;j<tileSet.at(i)->occupiers.size();j++) {
				testObj = tileSet.at(i)->occupiers.at(j);
				if(startsWith(toLower(testObj->getName()),toLower(_name))) {
					this->setTarget(testObj);
					return true;
				}
			}
		}
	}
	return false;
}

GameObject* Character::findObjectInRadius(const std::string &_name) {
	//Find a target with provided partial or complete name
	//If multiple matching targets are found cycle through them
	std::vector<Tile*> tileSet = TBAGame->gameWorld->getTilesInRadius(this->x,this->y,10); //placeholder until awareness stat
	GameObject* testObj;
	std::vector<GameObject*> allObjs;
	for(int i=0;i<tileSet.size();i++) {
		if(tileSet.at(i)->hasObjects() or tileSet.at(i)->isOccupied()) {
			allObjs = extend(tileSet.at(i)->objects,TBAGame->convert(tileSet.at(i)->occupiers));
			for(int j=0;j<allObjs.size();j++) {
				testObj = allObjs.at(j);
				if(startsWith(toLower(testObj->getName()),toLower(_name))) {
					return testObj;
				}
			}
		}
	}
	return nullptr;
}

void Character::setTargetAngle(Character *c) {
	//Sets target view angle to aim at target
	int newAng = atan2(c->y-this->y,c->x-this->x)*CONV_RADIANS;
	if(newAng < 0) newAng = 360+newAng;
	this->targetAng = newAng;
}	

std::vector<Character*> Character::getCharactersInRadius() {
	std::vector<Tile*> surroundingTiles = TBAGame->gameWorld->getTilesInRadius(this->x,this->y,10); //placeholder
	Tile* thisTile;

	std::vector<Character*> targets;

	for(int i=0;i<surroundingTiles.size();i++) {
		thisTile = surroundingTiles.at(i);
		if(thisTile->isOccupied()) {
			//extend(targets,thisTile->occupiers);
			for(int j=0;j<thisTile->occupiers.size();j++) {
				if((char*)this != (char*)thisTile->occupiers.at(j) and thisTile->occupiers.at(j)->isAlive()) targets.push_back(thisTile->occupiers.at(j));
			}
		}
		//if(thisTile->hasObjects()) extend(objects,thisTile->objects);
	}
	return targets;
}

std::vector<GameObject*> Character::getObjectsInRadius(objType type = OBJ_GENERIC) {
	//Find and return all objects with type "type" (or all objects if empty)
	// within small radius of caller (2)
	std::vector<Tile*> surroundingTiles = TBAGame->gameWorld->getTilesInRadius(this->x,this->y,10); //placeholder
	Tile* thisTile;

	bool noFilter = (type == OBJ_GENERIC);

	std::vector<GameObject*> objects;

	for(int i=0;i<surroundingTiles.size();i++) {
		thisTile = surroundingTiles.at(i);
		if(thisTile->hasObjects()) {
			//extend(objects,thisTile->occupiers);
			for(int j=0;j<thisTile->objects.size();j++) {
				if(noFilter or thisTile->objects.at(j)->type == type) objects.push_back(thisTile->objects.at(j));
			}
		}
		//if(thisTile->hasObjects()) extend(objects,thisTile->objects);
	}
	return objects;
}

bool Character::combatRetarget() {

	//Target highest priority target in awareness range (placeholder 10)
	std::vector<Character*> targets = this->getCharactersInRadius();
	Tile* thisTile;

	Character* occupant;

	for(int i=0;i<targets.size();i++) {
		occupant = targets.at(i);

		// Valid target conditions: 
		//	Target is alive
		//	Target is targeting you 
		//  Target is attacking, in combat with, or in pursuit of you
		//  If multiple targets meet these criteria, choose the closest

		ConditionSet target_valid = ConditionSet({
			occupant->isAlive(),
			occupant->hasTarget(),
			(char*)occupant->getCharTarget() == (char*)this,
			occupant->hasStatus(STATUS_COMBAT),
		});
		
		if(target_valid.valid()) {
			if(!this->hasTarget()) {
				this->setTarget(occupant);
			} else {
				//Check if this target is closer than current target
				this->setTarget(closer(this->getCharTarget(),occupant));
			}
		}
	}
	//ALWAYS RETURNS FALSE!!
	return this->hasTarget();
}

void Character::moveTo(Character *c) {
	//Set target angle to aim at c
	this->setTargetAngle(c);
	
	if(fabs(this->viewAng - this->targetAng) <= this->defaultFOV/2) {
		this->move_forward = true;
	} else {
		this->move_forward = false;
	}
}

void Character::moveAway(Character *c) {
	
	this->setTargetAngle(c);
	this->targetAng = ((int)this->targetAng+180)%360;
	this->move_forward = true;
}

//==========
//	 STATUS_MOVE
//==========

void Character::turn() {
	if(this->isPlayer and !this->autoMove) return;
	if((fabs(this->viewAng - this->targetAng)) > this->getTurnSpeed()) {
		
		int sign;
		if(this->targetAng > this->viewAng) {
			if( (360 - this->targetAng + this->viewAng) < this->targetAng-this->viewAng) {
				sign = -1;
			} else {
				sign = 1;
			}
		} else {
			if( (360 - this->viewAng + this->targetAng) < this->viewAng-this->targetAng) {
				sign = 1;
			} else {
				sign = -1;
			}
		}
		
		//																			Slow turn speed as viewAng approaches targetAng
		this->viewAng += this->getTurnSpeed()*sign*((float)360/(360-fabs(this->viewAng-this->targetAng)));

	} else {
		//If aim is close enough just set to target angle
		this->viewAng = this->targetAng;
	}

	//Wrap view angle between 0 and 360 for simplicity
	if(this->viewAng < 0) {
		this->viewAng = 360+this->viewAng;
	}
	this->viewAng = (int)this->viewAng%360;
}

void Character::setLocomotion() {

	if(!this->isPlayer) {
		if(this->hasTarget()) {
			if(this->hasStatus(STATUS_PURSUE)) {
				this->moveTo(this->getCharTarget());
			} else if(this->hasStatus(STATUS_ESCAPE)) {
				//Check awareness range here for targets in combat, targeting and moving towards this character
				//Using this character's target is a shortcut
				this->moveAway(this->getCharTarget());
			}
		}
	}

	if(fabs(this->velocity) < 0.5f) {
		this->removeStatus(STATUS_MOVE);
	}


}

//==========
//  STATUS_COMBAT
//==========

void Character::combat() {

	//====================
	//		ACQUIRE TARGET
	//====================

	if(!this->hasTarget()) {
		if(!this->combatRetarget()) {
			this->removeStatus(STATUS_COMBAT);
			return;
		}
	}

	//============================
	//		PREPARE STATUS_ATTACK CONDITIONS
	//============================

	if(!this->targetInRange()) {
		if(this->isPlayer) {
			//If target is out of range, treat as if character is not in combat
			// Resetting combat timer here prevents "Running attacks"
			// (Attacking immediately after entering combat range with pursuit target)
			// Provides a 1-hit advantage for combat-initiators
			
			//this->resetCombatTimer();
		} else {
			// If NPC's target is out of its range, move towards it 
			this->addStatus(STATUS_PURSUE);
		}
		return;
	}

	if(!this->getCharTarget()->isAlive()) {
		if(this->isPlayer) {
			//Placeholder for behavior when in combat with dead target
		} else {
			//Placeholder for behavior when in combat with dead target
		}
		this->removeStatus(STATUS_COMBAT);
		return;
	}
	//====================
	//		PREPARE STATUS_ATTACK
	//====================
	
	//Stop moving to prepare attack
	//Failsafe: If character enters preparation phase while attack is ready, reset attack timer to weapon swing time

	switch(this->getAttackStatus()) {
		//Character is in combat but not ready to attack
		case ATK_NOT_READY:
			this->addStatus(STATUS_COMBAT);
			return;
		//Character is currently attacking with weapon
		//Wait for attack to connect
		case ATK_STATUS_ATTACKING:
			this->addStatus(STATUS_ATTACK);
			return;
		//Swing makes contact, send attack and calculate damage
		case ATK_COMPLETE:
			this->removeStatus(STATUS_ATTACK);
			break;
	}


	//====================
	//		SEND ATTACK
	//====================

	this->sendAttack(this->target);

}

void Character::resetCombatTimer() {
	this->lastAttack = TBAGame->logicTicks;
}

attackStatus Character::getAttackStatus() {
	
	//Attack rate needs to be weapon/stat dependant
	if(TBAGame->logicTicks >= this->lastAttack + this->attackRate + this->defaultAttackSpeed) {
		return ATK_COMPLETE;
	} else if(TBAGame->logicTicks >= this->lastAttack + this->attackRate) {
		return ATK_STATUS_ATTACKING;
	}
	return ATK_NOT_READY;
}

itemType Character::getAttackType() {
	if(this->equipment->primary == nullptr) {
		return I_WEAPON_MELEE;
	}
	if(this->equipment->primary->hasType(I_WEAPON_MELEE)) return I_WEAPON_MELEE;
	return I_WEAPON_RANGED;
}

void Character::sendAttack(GameObject *target) {

	//Sends attack type and specifics to target
	//Returns actual damage dealt

	// Retrieve character's current damage output
	int damage = this->getAttackDamage();

	switch(this->getAttackType()) {
		case I_WEAPON_RANGED:
			float x,y;
			decompose(this->getLocation(),x,y);
			float tx,ty;
			decompose(this->target->getLocation(),tx,ty);
			// DEBUG:: Replace rand range with accuracy deviation and real projectile speed (bow, strength)
			// new Projectile(this,this->getLocation(),((-1+rand()%1)*CONV_DEGREES)+atan2(ty-y,tx-x),.5); //placeholder velocity
			new Projectile(this,this->getLocation(),((-1+rand()%1)+this->viewAng)*CONV_DEGREES,.5); //placeholder velocity
			break;
		case I_WEAPON_MELEE:
			// Send to target to be changed based on damage resistance
			static_cast<Character*>(target)->receiveAttack(damage,this);
			break;
	}

	// At this point the attack is complete
	this->lastAttack = TBAGame->logicTicks;

}

void Character::receiveAttack(int damage,GameObject *attacker) {
	//Takes attack type and specifics
	//Performs resistance calculation, returns damage dealt

	//If target is not in combat, put in combat
	if(!this->hasStatus(STATUS_COMBAT)) {
		this->addStatus(STATUS_COMBAT);
	}
	/*
	switch(this->getStatus()) {
		case STATUS_COMBAT:
		case STATUS_PURSUE:
		case STATUS_ATTACK:
			break;
		default:
			this->setStatus(STATUS_COMBAT);
	}
	*/
	
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
	// this->health -= damage;
	int targetLimb = rand()%101;
	if(targetLimb >= 87) {
		this->limbs.at(0).applyDamage(damage);
	} else if(targetLimb >= 47) {
		this->limbs.at(1).applyDamage(damage);
	} else if(targetLimb >= 27) {
		this->limbs.at(2).applyDamage(damage);
	}	else {
		this->limbs.at(3).applyDamage(damage);
	}

	this->checkLimbs();

	// Apply knockback
	// int magnitude = damage;
	// float directionX = -(this->getCharTarget()->x-this->x);
	// float directionY = this->getCharTarget()->y-this->y;
	// this->move({magnitude*directionX,magnitude*directionY});

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
	// REACT POST STATUS_ATTACK
	//====================

	if(this->health <= 0) {
		if(static_cast<Character*>(attacker)->isPlayer) TBAGame->displayText("\nKilled "+this->getName());
		this->kill();
		static_cast<Character*>(attacker)->setTarget(nullptr);
		static_cast<Character*>(attacker)->say("It's a shame, really");

		//If target is killled
		//Decide whether to disengage or choose new target
		//
		//Should check if targets in awareness range are targeting and attacking

		if(!static_cast<Character*>(attacker)->combatRetarget()) {
			//static_cast<Character*>(attacker)->setStatus(STATUS_IDLE);
		}

	}

}

void Character::checkLimbs() {
	for(int i=0;i<this->limbs.size();i++) {
		if((float)this->limbs.at(i).getHealth()/this->limbs.at(i).maxHealth <= .5) {
			this->addStatus(STATUS_CRIPPLED);
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
	
	// debug("1");
	
	if(!this->isAlive()) {
		//safeguard against zombies
		if(!this->hasStatus(STATUS_DEAD)) {
			this->kill();
		}
	} else {

		//Perform status-actions
		
		//Should eventually have more sophisticated criteria for escape
		if((float)this->health/this->maxHealth <= .5) {
			this->removeStatus(STATUS_COMBAT | STATUS_PURSUE);
			this->addStatus(STATUS_ESCAPE);
		}
		
		if(this->hasStatus(STATUS_COMBAT | STATUS_ATTACK) and !this->hasStatus(STATUS_MOVE | STATUS_PURSUE)) {
			this->combat();
		}
		
		if(this->hasStatus(STATUS_PURSUE)) {
			//Pursuing target until target is in range
			// At which point we begin combat
			if(this->targetInRange()) {
				this->removeStatus(STATUS_PURSUE);
				this->move_forward = false;	
			}
		}
		
		// if(this->hasStatus(STATUS_MOVE | STATUS_ESCAPE | STATUS_PURSUE)) {
			// this->setLocomotion();
		// }

		//Set movement-based status-actions
		// debug("2");
		this->setLocomotion();

		/*
		switch(this->getStatus()) {
			case STATUS_DEAD:
				break;
			case STATUS_IDLE:
				//Contemplate
				//this->decideOnNewAction()
				break;
			case STATUS_COMBAT:
			case STATUS_ATTACK:
				this->combat();
				break;
			case STATUS_PURSUE:
				if(this->targetInRange()) {
					this->addStatus(STATUS_COMBAT);
				}
				this->setLocomotion();
				break;
			case STATUS_MOVE:
				this->setLocomotion();
				break;
			case STATUS_ESCAPE:
				break;
		}
		*/
		if(this->viewAng != this->targetAng) this->turn();
	}
	
	
	
	//Physics
	this->move();
	
	

}

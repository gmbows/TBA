#include "Character.h"
#include "../common/Common.h"
#include "GameObject.h"
#include "../tools/Utility.h"
#include "FloatingText.h"
#include "Projectile.h"
#include "Squad.h"
#include <tuple>

std::map<StatusIndicator,const std::string> statusMap = {
	{STATUS_IDLE,"Idle"},
	{STATUS_ATTACK,"Swinging weapon"},
	{STATUS_NO_AMMO,"No ammo"},
	{STATUS_WORK,"Working"},
	{STATUS_COMBAT,"In combat"},
	{STATUS_PURSUE,"Pursuing target"},
	{STATUS_TRAVEL,"Traveling"},
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
// Get info of this character's target or None if applicable
std::string Character::getTargetInfo() {

	if(!this->hasTarget()) {return "None";}
	return this->getCharTarget()->getInfo();
	
}
// Get the name of this character's target or None if not applicable
std::string Character::getTargetName() {

	if(!this->hasTarget()) {return "None";}

	return this->target->getName();
}
// Set this character's target to o
void Character::setTarget(GameObject *o) {
	this->resetCombatTimer();
	this->target = o;
	if(o != nullptr and !this->isPlayer()) {
		this->setTargetAngle(this->getCharTarget());
	}	
}
// Return this character's target as a game object
Character* Character::getCharTarget()  {
	if(!this->hasTarget()) return nullptr;
	return static_cast<Character*>(this->target);
}
// Return this character's target
GameObject* Character::getTarget()  {
	return this->target;
}
//Determine if target is in range based on equipment
bool Character::targetInRange() {
	if(!this->hasTarget()) {
		return false;
	}
	if(this->getAttackRange() < 0) {
		return true;
	}
	float range = this->getAttackRange();
	return dist(this->getLocation(),this->getTarget()->getLocation()) <= range;
}
//Determine if work target is in range
bool Character::workTargetInRange() {
	if(!this->hasWorkTarget()) {
		return false;
	}
	int range = 1;
	return dist(this->getLocation(),this->workTarget->getLocation()) <= range;
}
//Set target to nearest character and return if target is different from existing target
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

				if(occupant->isAlive() and !occupant->isPlayer()) {
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
//Determine whether a character with name similar to _name is in awareness range
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
//Get list of all objects in awareness range
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
// Set targetAng to character
void Character::setTargetAngle(Character *c) {
	//Sets target view angle to aim at target
	int newAng = atan2(c->y-this->y,c->x-this->x)*CONV_RADIANS;
	if(newAng < 0) newAng = 360+newAng;
	this->targetAng = newAng;
}	
// Set targetAng to coordinates
void Character::setTargetLoc(int x,int y) {
	//Sets target view angle to aim at target
	int newAng = atan2(y-this->y,x-this->x)*CONV_RADIANS;
	// if(newAng < 0) newAng = 360+newAng;
	this->targetAng = newAng;
}	
//Get vector of characters in awareness range
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
// Get vector of objects of certain type in awareness range
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

//==========
//	STATUS
//==========
// Add status to current status flag
void Character::addStatus(StatusIndicator newStatus) {
	if(newStatus == STATUS_COMBAT and !this->hasStatus(STATUS_COMBAT)) {
		// Character must be stationary to enter combat
		this->removeStatus(STATUS_TRAVEL);
		this->resetCombatTimer();
	}
	if(newStatus != STATUS_IDLE) this->removeStatus(STATUS_IDLE);
	this->status = (this->status | newStatus);
}
// Set status to new status flag
void Character::setStatus(StatusIndicator newStatus) {
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

//==========
//	 TRAVEL
//==========

//Align viewAng with targetAng
void Character::turn() {
	// if(this->isPlayer and !this->autoMove) return;
	if((fabs(this->viewAng - this->targetAng)) > 2*this->getTurnSpeed()) {
		
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
		
		//Slow turn speed as viewAng approaches targetAng
		float diff = 180-fabs(this->viewAng-this->targetAng);
		if(diff == 0) {
			this->viewAng += this->getTurnSpeed()*sign;
		} else {
			this->viewAng += this->getTurnSpeed()*sign;//*fabs((float)180/diff);
		}

	} else {
		//If aim is close enough just set to target angle
		this->viewAng = this->targetAng;
	}

	//Wrap view angle between 0 and 360 for simplicity
	if(this->viewAng < 0) this->viewAng = 360+this->viewAng;
	this->viewAng = (int)this->viewAng%360;
}
//(Thinking method) Determine where to move to based on analysis of circumstances
void Character::setLocomotion() {

	if(this->hasTarget()) {
		if(this->hasStatus(STATUS_PURSUE)) {
			if(!this->targetInRange()) {
				if(TBAGame->gameWorld->hasSimplePath(this,this->getTarget())) {
					this->simple = true;
					this->targetPath = {this->getCharTarget()->location};
				} else {
					this->simple = false;
					this->generatePathTo(this->getTarget());
				}
				if(this->hasStatus(STATUS_COMBAT)) this->addStatus(STATUS_PURSUE);
			} else {
				this->targetPath.clear();
				this->removeStatus(STATUS_PURSUE | STATUS_TRAVEL);
			}
		} else if(this->hasStatus(STATUS_ESCAPE)) {
			//Check awareness range here for targets in combat, targeting this character
			//Using this character's target is a shortcut
			if(this->hasTarget()) {
				if(this->getCharTarget()->isAlive()) {
					this->moveAway(this->getCharTarget());
				} else {
					this->removeStatus(STATUS_ESCAPE);
				}
			}
		}
	}

	if(fabs(this->velocity) < 0.5f) {
		this->removeStatus(STATUS_TRAVEL);
	}

}
//Follow path if applicable
void Character::followPath() {
		this->addStatus(STATUS_TRAVEL);
		move_forward = true;
		int tx = this->targetPath.at(0)->x;
		int ty = -this->targetPath.at(0)->y;
		this->setTargetLoc(tx,ty);
		//Move to next path node if we are touching center of the current path
		if(dist({this->x,this->y},{tx,ty}) <= this->width) {
			this->targetPath.erase(this->targetPath.begin());
		}
		if(this->targetPath.size() == 0) {
			move_forward = false;
			this->removeStatus(STATUS_TRAVEL);
		}
}
//Generate path to character
void Character::generatePathTo(GameObject *o) {
	float x,y;
	decompose(o->getLocation(),x,y);
	this->generatePathTo(x,y);
}
//Generate path to tx,ty or adjacent block
bool Character::generatePathTo(float tx, float ty, bool adjacent) {
	if(TBAGame->logicTicks < this->lastPathCheck+TBAGame->pathCheckInterval) return false;
	// if(this->targetPath.size() > 0) return false;
	float g,h,f;
	float tg,th;
	
	Tile* targetTile = TBAGame->gameWorld->getTileAt(tx,ty);
	Tile *thisTile;
	Tile *testTile;
	Tile *currentBestTile = this->location;
	float currentBestF;
	
	float startingDist = dist({this->location->x,this->location->y},{tx,ty});

	std::vector<Tile*> bestPath;
	
	std::vector<Tile*> previousBests;

	thisTile = this->location;
	
	float tileX = this->location->x;
	float tileY = -this->location->y;
	
	int iter = 0;
	
	if(targetTile->isPassable() == false and adjacent == false) {
		debug(this->getName()+" cannot generate path to impassable tile "+std::to_string((int)tx)+", "+std::to_string((int)ty));
		this->lastPathCheck = TBAGame->logicTicks;
		return false;
	}
	
	while( !(tileX == tx and tileY == ty)) {
		
		currentBestF = 0xFF;
	
		for(int i=-1;i<=1;i++) {
			for(int j=-1;j<=1;j++) {
				if(j==0 and i==0) continue;
				if(tileX+j == this->location->x and tileY+i == -this->location->y) continue;
				if(contains(previousBests,TBAGame->gameWorld->getTileAt(tileX+j,tileY+i))) continue;
				if(!TBAGame->gameWorld->getTileAt(tileX+j,tileY+i)->isPassable()) continue;
				// std::cout << "Testing tile " << thisTile->x+j << ", " << thisTile->y+i << std::endl;
				// debug(thisTile->x+j);
				tg = dist({this->location->x,-this->location->y},{tileX+j,tileY+i});
				th = dist({tileX+j,tileY+i},{tx,ty});
				f = tg+th;
				// tiles.insert({thisTile,f});
				if(f < currentBestF) {
						// std::cout << "New best tile at " << testTile->x+j << ", " << testTile->y+i << std::endl;
						currentBestTile = TBAGame->gameWorld->getTileAt(tileX+j,tileY+i);
						currentBestF = f;
						previousBests.push_back(currentBestTile);
					
				}
				// std::cout << f << " \tat tile " << tileX+j << ", " << tileY+i << std::endl;
			}
		}
		tileX = currentBestTile->x;
		tileY = -currentBestTile->y;
		if((char*)thisTile == (char*)currentBestTile or iter > 40) {
			// debug("Pathing error");
			// this->targetPath = {};
			return false;
		}
		
		bestPath.push_back(currentBestTile);		
		iter++;
		
		if(adjacent) {
			if(targetTile->adjacent(currentBestTile)) break;
		}
		
	}
	
	if(!this->hasPath() or bestPath.size() <= this->targetPath.size()) this->targetPath = bestPath;
	
	this->lastPathCheck = TBAGame->logicTicks;
	
	for(int i=0;i<bestPath.size();i++) {
		
		// if(!bestPath.at(i)->hasBlocks()) bestPath.at(i)->addBlock(2);
		// std::cout << bestPath.at(i)->x << ", " << bestPath.at(i)->y << std::endl;
		
	}
	// debug(this->getName()+" is following new path ");
	// if(this->getName() != "Lost Bladesman") return true;
	for(int i=0;i<this->targetPath.size();i++) {
		// debug(std::to_string(this->targetPath.at(i)->x)+", "+std::to_string(this->targetPath.at(i)->y));
	}
	return true;
}
//Move away from character
void Character::moveAway(Character *c) {
	float x,y,tx,ty;
	// decompose(c->getLocation(),tx,ty);
	// decompose(this->getLocation(),x,y);
	// this->generatePathTo(x-tx,y-ty);
	this->setTargetAngle(c);
	this->targetAng = ((int)this->targetAng+180)%360;
	this->move_forward = true;
}

//==========
//  COMBAT
//==========

// (Thinking method) determine best target based on circumstances
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

	return this->hasTarget();
	
}
//Exit combat
void Character::exitCombat() {
	this->removeStatus(STATUS_NO_AMMO | STATUS_COMBAT | STATUS_ATTACK | STATUS_PURSUE);
	// this->removeStatus(STATUS_COMBAT);
	// this->removeStatus(STATUS_ATTACK);
	// this->removeStatus(STATUS_PURSUE);
}
//Select best choice of equipment of certain type 
bool Character::chooseNewEquipment(ItemType type) {
	std::vector<Item*> compatible = this->inventory->getItemsOfType(type);
	//DETERMINE BEST ITEM CHOICE HERE
	if(compatible.size() == 0) {
		return false;
	}
	return this->equip(compatible.at(0));
}
//Check equipment and equip new items 
bool Character::evaluateEquipment() {
	bool equipped = false;
	ItemType type;
	EquipmentSlot slot;
	std::vector<ItemType> implicitSlots = {I_WEAPON,I_ARMOR_HEAD,I_ARMOR_BODY,I_ARMOR_LEGS,I_ARMOR_FEET};
	std::vector<EquipmentSlot> slots = {EQUIP_PRIMARY,EQUIP_HEAD,EQUIP_BODY,EQUIP_LEGS,EQUIP_FEET};
	for(int i=0;i<implicitSlots.size();i++) {
		type = implicitSlots.at(i);
		slot = slots.at(i);
		if(!this->equipment->hasEquipped(slot)) {
			std::vector<Item*> compatible = this->inventory->getItemsOfType(type);
			if(compatible.size() == 0) continue;
			equipped = true;
			this->equipment->equip(compatible.at(0),slot);
		}
	}
	return equipped;
}
//Check statuses to determine if character is in combat
bool Character::inCombat() {
	return (this->hasStatus(STATUS_COMBAT) | this->hasStatus(STATUS_PURSUE));
}
//Think deeply about preparing for combat
void Character::combatThink() {
	if(this->equipment->hasEquipped(EQUIP_PRIMARY)) {
		//If this character has a primary weapon
		if(this->equipment->getSlot(EQUIP_PRIMARY)->hasType(I_WEAPON_RANGED)) {
			//If that primary weapon is a ranged weapon
			if(!this->hasAmmo((ItemType)this->equipment->getSlot(EQUIP_PRIMARY)->getWeaponType())) {
				//If we have ammo for that weapon
				this->addStatus(STATUS_NO_AMMO);
			} else {
				this->removeStatus(STATUS_NO_AMMO);
			}
			if(this->hasTarget()) {
				if(this->hasStatus(STATUS_NO_AMMO)) {
					//If we have a target and no ammo, swap to a melee weapon and pursue them
					if(this->chooseNewEquipment(I_WEAPON_MELEE)) {
						this->removeStatus(STATUS_NO_AMMO);
					}
				}
			}
		} else if(this->equipment->getSlot(EQUIP_PRIMARY)->hasType(I_WEAPON_MELEE)) {
			if(this->hasTarget()) {
				if(!this->targetInRange()) {
					//Select ranged weapon with ammo and equip
				}
			}
		}
	}
}
//Prepare combat environment and engage in combat
void Character::combat() {

	//Wait until we are stationary to initiate combat
	if(this->hasStatus(STATUS_TRAVEL)) return;
	// if(this->hasStatus(STATUS_PURSUE)) return;

	//====================
	//		ACQUIRE TARGET
	//====================

	if(!this->hasTarget()) {
		if(!this->combatRetarget()) {
			this->exitCombat();
			return;
		}
	}

	//============================
	//		PREPARE ATTACK CONDITIONS
	//============================

	if(!this->targetInRange()) {
			// If NPC's target is out of its range, move towards it 
		this->addStatus(STATUS_PURSUE);
		return;
	}

	this->setTargetAngle(this->getCharTarget());

	if(!this->getCharTarget()->isAlive()) {
		if(this->isPlayer()) {
			//Placeholder for behavior when in combat with dead target
		} else {
			//Placeholder for behavior when in combat with dead target
		}
		this->exitCombat();
		return;
	}

	//====================
	//		PREPARE ATTACK
	//====================

	//Stop moving to prepare attack
	//Failsafe: If character enters preparation phase while attack is ready, reset attack timer to weapon swing time

	this->combatThink();

	switch(this->getAttackStatus()) {
		//Character is in combat but not ready to attack
		case ATK_NOT_READY:
			this->addStatus(STATUS_COMBAT);
			return;
		//Character is currently attacking with weapon
		//Wait for attack to connect
		case ATK_STATUS_ATTACKING:
			if(!this->hasStatus(STATUS_NO_AMMO)) this->addStatus(STATUS_ATTACK);
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
//Force character to restart combat actions 
void Character::resetCombatTimer() {
	this->lastAttack = TBAGame->logicTicks;
}
//Get status of attacking character (preparing to attack, swinging weapon, attack complete)
attackStatus Character::getAttackStatus() {

	//Attack rate needs to be weapon/stat dependant
	if(TBAGame->logicTicks >= this->lastAttack + this->attackRate + this->defaultAttackSpeed) {
		return ATK_COMPLETE;
	} else if(TBAGame->logicTicks >= this->lastAttack + this->attackRate) {
		return ATK_STATUS_ATTACKING;
	}
	return ATK_NOT_READY;
}
//Determine attack type based on equipment 
ItemType Character::getAttackType() {
	if(this->isUnarmed()) {
		return I_WEAPON_MELEE;
	}
	if(this->equipment->getSlot(EQUIP_PRIMARY)->hasType(I_WEAPON_MELEE)) return I_WEAPON_MELEE;
	return I_WEAPON_RANGED;
}
//Determine if character has ammo for ItemType
bool Character::hasAmmo(ItemType type) {
	for(int i=0;i<this->inventory->contents->size();i++) {
		if(this->inventory->contents->at(i)->hasType(I_AMMO | type)) return true;
	}
	return false;
}
//Retrieve ammo object for ItemType
Item* Character::getAmmo(ItemType type) {
	for(int i=0;i<this->inventory->contents->size();i++) {
		if(this->inventory->contents->at(i)->hasType(I_AMMO | type)) return this->inventory->contents->at(i);
	}
	return nullptr;
}
//Attack target
void Character::sendAttack(GameObject *target) {

	//Sends attack type and specifics to target
	//Returns actual damage dealt

	// Retrieve character's current damage output
	int damage = this->getAttackDamage();

	switch(this->getAttackType()) {
		case I_WEAPON_RANGED:
			// float x,y;
			// decompose(this->getLocation(),x,y);
			// float tx,ty;
			// decompose(this->target->getLocation(),tx,ty);
			// DEBUG:: Replace rand range with accuracy deviation and real projectile speed (bow, strength)
			// new Projectile(this,this->getLocation(),((-1+rand()%1)*CONV_DEGREES)+atan2(ty-y,tx-x),.5); //placeholder velocity
			//												accuracy mult goes here \/
			if(this->hasAmmo(I_WEAPON_BOW)) {
				float inaccuracy = 12;
				inaccuracy = (-(inaccuracy/2)+rand()%(int)inaccuracy);
				new Projectile(this,this->getLocation(),damage,(inaccuracy+this->viewAng)*CONV_DEGREES,.5); //placeholder velocity
				this->inventory->remove(this->getAmmo(I_WEAPON_BOW));
			} else {
				this->addStatus(STATUS_NO_AMMO);
			}
			break;
		case I_WEAPON_MELEE:
			// Send to target to be changed based on damage resistance
			static_cast<Character*>(target)->receiveAttack(damage,this);
			break;
	}

	// At this point the attack is complete
	this->lastAttack = TBAGame->logicTicks;

}
//Receive attack from attacker
void Character::receiveAttack(int damage,GameObject *attacker) {
	//Takes attack type and specifics
	//Performs resistance calculation, returns damage dealt

	//If target is not in combat, put in combat
	if(!this->hasStatus(STATUS_COMBAT)) {
		this->addStatus(STATUS_COMBAT);
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

	//Alert player squad
	if(this->hasSquad()) {
		this->squad->alert(attacker);
	}

	//Apply damage resistance calculation
	// this->health -= damage;
	float def;
	int limbRoll = rand()%101;
	LimbType targetLimb;
	EquipmentSlot defenseSlot;
	if(limbRoll >= 87) {
		defenseSlot = EQUIP_HEAD;
		targetLimb = LIMB_HEAD;
	} else if(limbRoll >= 47) {
		defenseSlot = EQUIP_BODY;
		targetLimb = LIMB_TORSO;
	} else if(limbRoll >= 27) {
		defenseSlot = EQUIP_BODY;
		targetLimb = LIMB_ARMS;
	}	else {
		defenseSlot = EQUIP_LEGS;
		targetLimb = LIMB_LEGS;
	}

	def = this->equipment->getDef(defenseSlot);
	damage = std::max(0.0f,damage-def);

	this->body->getLimb(targetLimb)->applyDamage(damage-def);

	this->checkBody();

	// Apply knockback
	// int magnitude = damage;
	// float directionX = -(this->getCharTarget()->x-this->x);
	// float directionY = this->getCharTarget()->y-this->y;
	// this->move({magnitude*directionX,magnitude*directionY});

	//Damage popups
	std::string damageIndicator = std::to_string(damage)+" ("+this->body->toString(targetLimb)+")";
	if(this->isPlayer()) {
		// new FloatingText(3,{damageIndicator},{this->location->x,-this->location->y},{160,20,20,255});
		TBAGame->displayText("\nReceived "+std::to_string(damage)+" damage from "+attacker->getName());
	} else {
		// new FloatingText(3,{damageIndicator},{this->location->x,-this->location->y},{20,160,20,255});
		// new FloatingText(3,{"Blessings of King upon you"},this->getAbsoluteLocation(),this);
	}

	if(static_cast<Character*>(attacker)->isPlayer()) {
		TBAGame->displayText("\nDealt "+std::to_string(damage)+" damage to "+this->getEntityName());
	}


	//====================
	// REACT POST STATUS_ATTACK
	//====================

	if(this->health <= 0) {
		//Globally broadcasted kill notifs
		TBAGame->displayText("\n"+attacker->getEntityName()+" kills "+this->getEntityName());
		if(static_cast<Character*>(attacker)->isPlayer()) TBAGame->displayText("\nKilled "+this->getName());
		this->kill();
		//If killed character was killer's actual target
		if((char*)this == (char*)static_cast<Character*>(attacker)->getTarget()) {
			static_cast<Character*>(attacker)->setTarget(nullptr);
			static_cast<Character*>(attacker)->say("It's a shame, really");
		} else {
			static_cast<Character*>(attacker)->say("Oops!");
		}

		//If target is killled
		//Decide whether to disengage or choose new target
		//
		//Should check if targets in awareness range are targeting and attacking

		if(!static_cast<Character*>(attacker)->combatRetarget()) {
			//static_cast<Character*>(attacker)->setStatus(STATUS_IDLE);
		}

	}

}
//Check limb health/status
void Character::checkBody() {
	if(this->body->getVitality() <= .2) {
		if(this->isAlive()) this->kill();
		return;
	}
	for(int i=0;i<this->body->getLimbs().size();i++) {
		if((float)this->body->getLimbs().at(i)->getHealth()/this->body->getLimbs().at(i)->maxHealth <= .5) {
			this->addStatus(STATUS_CRIPPLED);
			if(this->hasStatus(STATUS_COMBAT)) {
				this->exitCombat();
				this->addStatus(STATUS_ESCAPE);
			}
		}
	}
}

//==========
//		MISC
//==========
//Say message
void Character::say(const std::string& message) {
	TBAGame->displayText("\n"+this->getName()+" says \""+message+"\"");
	new FloatingText(3,TBAGame->gameWindow->textScreen->prepareCommandForDisplay(message),this->getApproximateLocation(),this);
	// new FloatingText(3,message,this->getLocation(),this);
}

//==========
//	  UPDATE
//==========

void Character::think() {
	//Perform status-actions
	
	//Should eventually have more sophisticated criteria for escape
	if(this->body->getVitality() <= .5) {
		this->exitCombat();
		this->addStatus(STATUS_ESCAPE);
	}

	if(this->inCombat()) {
		this->combat();
	}

	if(this->hasStatus(STATUS_WORK) and this->hasWorkTarget()) {
		this->processWork();
	}
	
}

void Character::update() {
		
	//Process status effects
	this->processEffects();
	
	if(this->isAlive()) {
		this->think();
	} else {
		//safeguard against zombies
		if(!this->hasStatus(STATUS_DEAD)) {
			this->kill();
		}
		return;
	}
	if(this->viewAng != this->targetAng) this->turn();
	// if(this->name == "Lost Bladesman") debug("Aim: "+std::to_string(this->targetAng));
	
	if(this->targetPath.size() > 0) this->followPath();
	
	//Physics
	this->setLocomotion();
	this->move();

}

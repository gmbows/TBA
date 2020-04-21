#include "Character.h"
#include "../common/Common.h"
#include "StatusEffect.h"
#include "../tools/Utility.h"
#include "GameObject.h"
#include "Inventory.h"
#include "Behavior.h"
#include "Equipment.h"
#include "ResourceNode.h"
#include "Statistics.h"
#include "StatusEffect.h"
#include "../common/Tile.h"
#include "ItemManifest.h"

#include <cmath>
#include <tuple>
#include <random>

Character::Character(bool player, int capacity, const std::string& _name, float _x,float _y): x(_x), y(_y), isPlayer(player), name(_name), GameObject(OBJ_CHARACTER) {

	this->location = TBAGame->gameWorld->getTileAt(_x,_y);

	this->stats = new StatSet();
	this->equipment = new Equipment();

	this->init_stats();

	this->setStatus(STATUS_IDLE);
	this->target = nullptr;

	if(player) {
		this->maxHealth = 40;
		this->attackRate -= 20;
		TBAGame->playerChar = this;
		this->maxMoveSpeed = .5;
		this->displayID=3;
		this->traction = 2;
	} else {
		//Random movespeeds
		this->maxMoveSpeed = .3+(.1*((rand()%2)-1)); //10+-5
		this->displayID=6;
		this->traction = 1;
	}
	this->health = this->maxHealth;
	this->inventory = new Inventory(capacity);
	
	for(int i=0;i<10;i++) {
		this->inventory->add(1+rand()%(itemManifest.size()-1));
	}
	
	this->setLocation(x,y);
}

//==========
//	MOVEMENT
//==========

void Character::resolveMove(float &newX, float &newY) {
	if(newX+this->width >= TBAGame->gameWorld->size) {
		newX = this->x;
	}
	if(newY+this->width >= TBAGame->gameWorld->size) {
		newY = this->y;
	}
	
	bool YP = TBAGame->gameWorld->getTileAt(this->x,newY+this->width)->isPassable();
	bool YN = TBAGame->gameWorld->getTileAt(this->x,newY-this->width)->isPassable();
	bool XP = TBAGame->gameWorld->getTileAt(newX+this->width,this->y)->isPassable();
	bool XN = TBAGame->gameWorld->getTileAt(newX-this->width,this->y)->isPassable();
	
	if(!YP) {
		if(newY > this->y) newY = this->y;
	}
	if(!YN) {
		if(newY < this->y) newY = this->y;
	}
	if(!XP) {
		if(newX > this->x) newX = this->x;
	}
	if(!XN) {
		if(newX < this->x) newX = this->x;
	}

}

void Character::setLocation(float newX,float newY) {
	this->location->evict(this);
	this->location = TBAGame->gameWorld->getTileAt(newX,newY);
	this->location->occupyWith(this);
}

void Character::move() {

	float newX = this->x+(std::cos(this->viewAng*CONV_DEGREES)*((float)this->velocity*(TBAGame->logicTicks - this->lastMove)*TBAGame->moveSpeedUnit));
	float newY = this->y+(std::sin(this->viewAng*CONV_DEGREES)*((float)this->velocity*(TBAGame->logicTicks - this->lastMove)*TBAGame->moveSpeedUnit));

	if(this->autoMove) {
		this->velocity += this->maxMoveSpeed;
	} else {
		if(this->move_forward) this->velocity += this->maxMoveSpeed;
		if(this->move_back) this->velocity -= this->maxMoveSpeed;
	}

	//If character is trying to move into a new space
	this->resolveMove(newX,newY);

	this->setLocation(newX,newY);

	this->x = newX;
	this->y = newY;

	//Velocity decay
	this->velocity *= .9;
	
	if(this->velocity == 0) this->removeStatus(STATUS_TRAVEL);

	this->lastMove = TBAGame->logicTicks;

}

//===================
//	  EQUIPMENT / STATS
//===================

bool Character::isUnarmed() {
	return (this->equipment->primary == nullptr and this->equipment->secondary == nullptr);
}

int Character::getAttackDamage() {
	if(this->isUnarmed()) {
		return this->defaultAttackDamage;
	}
	return this->equipment->primary->getAttribute(ATTRIB_DAMAGE);
}

float Character::getAttackRange() {
	if(this->isUnarmed()) {
		return this->defaultAttackRange;
	}
	return this->equipment->primary->getAttribute(ATTRIB_RANGE);
}

std::string Character::getInvString() {
	return this->name+":"+this->inventory->toString();
}

bool Character::equip(Item *item) {
	//Item is a weapon, placeholder for secondary equipping
	if(item->hasType(I_WEAPON)) {
		this->equipment->primary = item;
		return true;
	}
	if(item->hasType(I_ARMOR)) {
		if(item->hasType(I_ARMOR_HEAD)) {
			this->equipment->head = item;
		} else if(item->hasType(I_ARMOR_BODY)) {
			this->equipment->body = item;
		} else if(item->hasType(I_ARMOR_LEGS)) {
			this->equipment->legs = item;
		} else if(item->hasType(I_ARMOR_FEET)) {
			this->equipment->feet = item;
		}
		return true;
	}
	return false;
}

bool Character::plant(Item *item) {
	//Item is a weapon, placeholder for secondary equipping
	if(item->hasType(I_PLANTABLE)) {
		this->location->planted = true;
		this->inventory->remove(item);
		return true;
	}
	return false;
}

bool Character::consume(Item *item) {
	//Trigger various effects from item on this character
	if(item->hasType(I_CONSUMABLE)) {
		this->triggerItemEffects(item,ACTION_CONSUME);
		this->inventory->remove(item);
		return true;
	}
	return false;
}

void Character::triggerItemEffects(Item* item,Action action) {
	std::vector<StatusEffect*> effectsOnAction = item->getEffectsOnAction(action);
	for(int i=0;i<item->effects.size();i++) {
		this->effects.push_back(effectsOnAction.at(i));
		// debug("Added effect to "+this->getName()+" with magnitude "+std::to_string(item->effects.at(i)->magnitude));
	}
}

void Character::processEffects() {
	for(int i=0;i<this->effects.size();i++) {
		//Apply effects or remove if inactive
		if(!this->effects.at(i)->applyEffect(this)) {
			this->effects.erase(this->effects.begin()+i);
		}
	}
}

bool Character::work(GameObject* node) {
	if(node->type == OBJ_INTERACTIVE) {
		this->addStatus(STATUS_WORK);
		this->workTarget = node;
		return true;
	} else {
		return false;
	}
}

void Character::processWork() {
	if(this->hasWorkTarget()) {
		this->workTarget->getAsResource()->work();
	}
}

//=============
// MISC / CLEANUP
//==============

std::tuple<float,float> Character::getApproximateLocation() {

	float x = TBAGame->gameWindow->mapScreen->x+(TBAGame->gameWindow->mapScreen->w/2)-(TBAGame->gameWindow->mapScreen->charW*(TBAGame->playerChar->location->x - this->location->x));
	float y = TBAGame->gameWindow->mapScreen->y+(TBAGame->gameWindow->mapScreen->h/2)-(TBAGame->gameWindow->mapScreen->charH*(TBAGame->playerChar->location->y + this->location->y));

	return {x,y};

}

std::tuple<float,float> Character::getAbsoluteLocation() {

	float x = TBAGame->gameWindow->mapScreen->x+(TBAGame->gameWindow->mapScreen->w/2)-(TBAGame->gameWindow->mapScreen->charW*(TBAGame->playerChar->x - this->x));
	float y = TBAGame->gameWindow->mapScreen->y+(TBAGame->gameWindow->mapScreen->h/2)-(TBAGame->gameWindow->mapScreen->charH*(TBAGame->playerChar->y - this->y));

	return {x,y};

}

std::string Character::getStatusString() {
	//std::string statusString;
	std::vector<std::string> statuses;
	//return statusString;
	flag statFlag;
	for(int i=0;i<log2((flag)STATUS_END);i++) {
		statFlag = 1 << i;
		if(this->hasStatus((statusIndicator)statFlag)) {
			statuses.push_back(statusMap.at((statusIndicator)statFlag));
		}
	}
	if(statuses.size() == 0) {
		return "";
	}
	return "\t"+join("\n\t...\t",statuses);
	//return statusString;
}

std::string Character::getInfo() {
	
	std::string info = " Name:\t"+this->name + "\n" +
				"\tStatus:"+this->getStatusString() + "\n" ;
				// "\tAim Angle:"+std::to_string(this->viewAng) + "\n" +
				// "\tTarget Angle:"+std::to_string(this->targetAng) + "\n" ;
				// "\tLocation:\t"+std::to_string((int)std::round(this->x)) + "," + std::to_string((int)std::round(this->y)) + "\n";
				if(this->hasTarget()) {
					info += "\tTarget:\t"+this->getTargetName() + "\n";
				}
				//"\tVelcocity:\t"+std::to_string(this->velocityX) + "," + std::to_string(this->velocityY) + "\n" +
				// info += "\tHealth:\t"+std::to_string(this->health) + "/" + std::to_string(this->maxHealth) + "\n\n" + 
				// info += "\n\n"+ //"\tAttack rate: "+std::to_string(this->attackRate) + "\n\n" +
				info += "\n\tHead: "+std::to_string(this->limbs.at(0)->getHealth()) + "/" + std::to_string(this->limbs.at(0)->maxHealth) + "\n"+
				"\tBody: "+std::to_string(this->limbs.at(1)->getHealth()) + "/" + std::to_string(this->limbs.at(1)->maxHealth) + "\n"+
				"\tArms: "+std::to_string(this->limbs.at(2)->getHealth()) + "/" + std::to_string(this->limbs.at(2)->maxHealth) + "\n"+
				"\tLegs: "+std::to_string(this->limbs.at(3)->getHealth()) + "/" + std::to_string(this->limbs.at(3)->maxHealth);
					
				if(this->hasEffects()) {
					info += "\n\nStatus Effects:";
					for(int i=0;i<this->effects.size();i++) {
						info += "\n "+this->effects.at(i)->getTypeString()+" ("+this->effects.at(i)->getDurationString()+")";
					}
				}
	return info;
}

void Character::kill() {
	if(this->health > 0) { this->health = 0;}
	this->move_forward = false;
	this->move_back = false;
	//a warrior's death
	//this->target = nullptr;
	this->setStatus(STATUS_DEAD);
}

void Character::cleanup() {
	this->location->evict(this);
	this->target = nullptr;
	this->location = nullptr;
	delete this->inventory;
	if(this->isPlayer) {
		new Character(true,160,"Generic",0,0);
	}
	TBAGame->removeObject(this);
	delete this;
}
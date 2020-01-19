#include "Character.h"
#include "../common/Common.h"
#include "../tools/Utility.h"
#include "GameObject.h"
#include "Inventory.h"
#include "Behavior.h"
#include "Equipment.h"
#include "Statistics.h"
#include "../common/Tile.h"

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
	this->setLocation(x,y);
}

//==========
//	MOVEMENT
//==========

void Character::resolveMove(float &newX, float &newY) {
	if(newX >= TBAGame->gameWorld->size) {
		newX = this->x;
	}
	if(newY >= TBAGame->gameWorld->size) {
		newY = this->y;
	}
	Tile *testLocY = TBAGame->gameWorld->getTileAt(this->x,newY);
	Tile *testLocX = TBAGame->gameWorld->getTileAt(newX,this->y);
	if(!testLocY->isPassable()) {
		newY = this->y;
	}
	if(!testLocX->isPassable()) {
		newX = this->x;
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

	if(this->velocity == 0) return;

	
	
	//If character is trying to move into a new space
	this->resolveMove(newX,newY);

	this->setLocation(newX,newY);

	this->x = newX;
	this->y = newY;

	//Velocity decay
	this->velocity *= .9;

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

//=============
// MISC / CLEANUP
//==============

std::tuple<float,float> Character::getApproximateLocation() {

	float x = TBAGame->gameWindow->mapScreen->x+(TBAGame->gameWindow->mapScreen->w/2)-(TBAGame->gameWindow->mapScreen->charW*(TBAGame->playerChar->x - this->location->x));
	float y = TBAGame->gameWindow->mapScreen->y+(TBAGame->gameWindow->mapScreen->h/2)-(TBAGame->gameWindow->mapScreen->charH*(TBAGame->playerChar->y + this->location->y));

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
	
	std::string info = " \n\n Name:\t"+this->name + "\n" +
				"\tStatus:"+this->getStatusString() + "\n" ;
				// "\tAim Angle:"+std::to_string(this->viewAng) + "\n" +
				// "\tTarget Angle:"+std::to_string(this->targetAng) + "\n" ;
				// "\tLocation:\t"+std::to_string((int)std::round(this->x)) + "," + std::to_string((int)std::round(this->y)) + "\n";
				if(this->hasTarget()) {
					info += "\tTarget:\t"+this->getTargetName() + "\n";
				}
				//"\tVelcocity:\t"+std::to_string(this->velocityX) + "," + std::to_string(this->velocityY) + "\n" +
				info += "\tHealth:\t"+std::to_string(this->health) + "/" + std::to_string(this->maxHealth) + "\n\n" + 
				"\tAttack rate: "+std::to_string(this->attackRate);
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
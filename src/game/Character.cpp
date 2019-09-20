#include "Character.h"
#include "../common/Common.h"
#include "../tools/StringFuncs.h"
#include "GameObject.h"
#include "Inventory.h"
#include "Behavior.h"
#include "Equipment.h"
#include "Statistics.h"
#include "../common/Tile.h"

#include <tuple>
#include <random>

Character::Character(bool player, int capacity, const std::string& _name, float _x,float _y): x(_x), y(_y), isPlayer(player), name(_name), GameObject(OBJ_CHARACTER) {

	this->location = TBAGame->gameWorld->getTileAt(x,y);

	this->stats = new StatSet();
	this->equipment = new Equipment();

	this->attackRate = this->defaultAttackRate + (-500+rand()%1000);

	this->setStatus(IDLE);
	this->target = nullptr;

	if(player) {
		this->equipment->primary = new Item(4);
		this->maxHealth = 40;
		this->attackRate -= 1000;
		TBAGame->playerChar = this;
		this->maxMoveSpeed = 15; 
		this->displayID=3;
		this->traction = 1;
	} else {
		//Random movespeeds
		this->maxMoveSpeed = 10+((rand()%2)-1); //10+-5
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

bool Character::resolveMove(float &newX, float &newY) {

	bool changed = false;

	Tile* thisTile = TBAGame->gameWorld->getTileAt(this->x,newY);
	//If movement on Y axis results in occupation conflict
	//Set Y movement to 0
	if(!thisTile->isPassable() or thisTile->invalid) {
		if(thisTile != this->location) {
			newY = this->y;
			this->velocityY = 0;
			changed = true;
		}
	}

	thisTile = TBAGame->gameWorld->getTileAt(newX,this->y);
	//If movement on X axis results in occupation conflict
	//Set X movement to 0
	if(!thisTile->isPassable() or thisTile->invalid) {
		if(thisTile != this->location) {
			newX = this->x;
			this->velocityX = 0;
			changed = true;
		}
	}

	return changed;

}

void Character::setLocation(float newX,float newY) {
	this->location->evict(this);
	this->location = TBAGame->gameWorld->getTileAt(newX,newY);
	this->location->occupyWith(this);
}

void Character::move(std::tuple<int,int> direction) {
	float newX = this->x+((float)this->velocityX*(SDL_GetTicks() - this->lastMove)*TBAGame->moveSpeedUnit);
	float newY = this->y-((float)this->velocityY*(SDL_GetTicks() - this->lastMove)*TBAGame->moveSpeedUnit);

	int dx = std::get<0>(direction);
	int dy = std::get<1>(direction);

	this->velocityX += dx;
	this->velocityY += dy;

	
	Tile* thisTile = TBAGame->gameWorld->getTileAt(newX,newY);


	//If character is trying to move into a new space
	if(thisTile != this->location) {
		//If new space is occupied or otherwise impassable
		if(!thisTile->isPassable() or !TBAGame->gameWorld->locationIsValid(newX,newY)) {

			if(!this->resolveMove(newX,newY)) {
				//If move cannot be resolved to free space, cancel movement
				return;
			}
		}	
		//Move character to new location and vacate current tile
		this->setLocation(newX,newY);
	}

	this->x = newX;
	this->y = newY;

	//Resistance
	if(dx == 0) {
		this->velocityX *= 1-(this->location->roughness);
	}
	if(dy == 0) {
		this->velocityY *= 1-(this->location->roughness);
	}

	this->lastMove = SDL_GetTicks();

	//if(this->velocityX == 0) {this->setStatus(IDLE);}
	//if(this->velocityY == 0) {this->setStatus(IDLE);}
	

	//this->velocityX = (std::fabs(this->velocityX) < TBAGame->moveSpeedUnit)? 0 : this->velocityX;
	//this->velocityY = (std::fabs(this->velocityY) < TBAGame->moveSpeedUnit)? 0 : this->velocityY;
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
	return this->equipment->primary->getAttribute(DAMAGE);
}

float Character::getAttackRange() {
	if(this->isUnarmed()) {
		return this->defaultAttackRange;
	}
	return this->equipment->primary->getAttribute(RANGE);
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


std::string Character::getInfo() {

	return " \n\n Name:\t"+this->name + "\n" +
				"\tStatus:\t"+statusMap.at(this->status) + "\n" +
				"\tLocation:\t"+std::to_string((int)std::round(this->x)) + "," + std::to_string((int)std::round(this->y)) + "\n" +
				"\tTarget:\t"+this->getTargetName() + "\n" +
				//"\tVelcocity:\t"+std::to_string(this->velocityX) + "," + std::to_string(this->velocityY) + "\n" +
				"\tHealth:\t"+std::to_string(this->health) + "/" + std::to_string(this->maxHealth) + "\n\n" + 
				"\tAttack rate: "+std::to_string(this->attackRate);

}

void Character::kill() {
	if(this->health > 0) { this->health = 0;}
	this->direction = {0,0};
	this->target = nullptr;
	this->setStatus(DEAD);
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
#include "Projectile.h"
#include "GameObject.h"
#include "../common/Common.h"
#include "../tools/Utility.h"
#include "Squad.h"
#include "GameObject.h"

#include <SDL2/SDL.h>

Projectile::Projectile(GameObject* _owner, std::tuple<float,float> _location,float dmg,float _ang,float _velocity): owner(_owner), damage(dmg), angle(_ang), velocity(_velocity), GameObject(OBJ_PROJECTILE) {

	float _x,_y;
	decompose(_location,_x,_y);

	this->x = _x+(this->collisionSize*std::cos(this->angle));
	this->y = _y+(this->collisionSize*std::sin(this->angle));

	this->location = TBAGame->gameWorld->getTileAt(this->x,this->y);
	this->location->addObject(this);

	this->lastUpdate = TBAGame->logicTicks;
	this->destroyTime = TBAGame->logicTicks + this->maxAge;

	this->active = true;

}

void Projectile::update() {
	

	if(TBAGame->logicTicks >= this->destroyTime) {
		this->cleanup();
	}
	

	if(this->active) {
		this->relocate();
	} else if(this->trackSubject != nullptr) {
		decompose(trackSubject->getLocation(),this->x,this->y);
	}
	

	//Location correction
	if((char*)TBAGame->gameWorld->getTileAt(this->x,this->y) != (char*)this->location) {
		this->location->removeObject(this);
		this->location = TBAGame->gameWorld->getTileAt(this->x,this->y);
		this->location->objects.push_back(this);
	}
	

	this->lastUpdate = TBAGame->logicTicks;
}

void Projectile::relocate() {

	float testX = this->x;
	float testY = this->y;

	Tile* thisTile = TBAGame->gameWorld->getTileAt(testX,testY);
	Character* occupant;

	//Interpolate between old and new locations to check collision
	for(int j=0;j<(TBAGame->logicTicks-this->lastUpdate)*4;j++) {
		// Check collision with impassable block or tile
		// Embed a bit into surface
		if(!thisTile->isPassable()) {
			// debug((((rand()%10)+10)/100.0f)*(1+this->velocity)*std::sin(this->angle));
			this->x = testX - (((rand()%10)+10)/100.0f)*(1+this->velocity)*std::cos(this->angle);
			this->y = testY - (((rand()%10)+10)/100.0f)*(1+this->velocity)*std::sin(this->angle);
			this->active = false;
			return;
		}
		// Check collision with characters
		if(thisTile->isOccupied()) {
			for(int i=0;i<thisTile->occupiers.size();i++) {

				occupant = thisTile->occupiers.at(i);

				// Do not collide with creator or dead characters or squad members
				if((char*)occupant == (char*)this->owner) continue;
				if(!occupant->isAlive()) continue;
				if(owner->getAsCharacter()->hasSquad()) {
					if(owner->getAsCharacter()->squad->isMember(occupant)) continue;
				}

				if(dist(occupant->getLocation(),{testX,testY}) <= this->collisionSize) {
					//Arrow damage
					occupant->receiveAttack(this->damage,this->owner);
					this->x = testX - (((rand()%10)+10)/100.0f)*(1+this->velocity)*std::cos(this->angle);
					this->y = testY - (((rand()%10)+10)/100.0f)*(1+this->velocity)*std::sin(this->angle);
					this->trackSubject = occupant;
					this->active = false;
					return;

				}
			}
		}
		// Increment interpolation
		testX += (this->velocity/4)*std::cos(this->angle);
		testY += (this->velocity/4)*std::sin(this->angle);
		thisTile = TBAGame->gameWorld->getTileAt(testX,testY);
		if((char*)thisTile != (char*)this->location) {
			this->location->removeObject(this);
			this->location = thisTile;
			this->location->addObject(this);
		}
	}
	this->x = testX;
	this->y = testY;
}

void Projectile::cleanup() {
	// debug("Cleaning up projectile");
	this->location->removeObject(this);
	// TBA removeObject calls destructor, no need to delete
	TBAGame->removeObject(this);
	// debug("Deleting object");
	// delete this;
	// debug("Done Cleaning up projectile");
}
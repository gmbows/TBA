#include "Projectile.h"
#include "GameObject.h"
#include "../common/Common.h"
#include "../tools/Utility.h"
#include "Squad.h"
#include "GameObject.h"

#include "../../../shared/Shared.h"

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
	
	TBAGame->generatePacket(EVENT_PROJ_CREATE,this->serialize());
}

std::string Projectile::serialize() {
	std::string proj;
	//	00000000 00 00 000|c1|c2|c3|c4|...
	std::string type = std::to_string((int)this->type);
	std::string display = std::to_string(this->getDisplayID());
	std::string id = std::to_string(this->objectID);
	
	std::string sactive = std::to_string((int)this->active);
	std::string ang = std::to_string((int)(this->angle*100));
	std::string vel = std::to_string((int)(this->velocity*100));
	std::string x = std::to_string((int)(this->x*100));
	std::string y = std::to_string((int)(this->y*100));
	//ID
	pad(type,'0',PAD_SHORT);
	pad(display,'0',PAD_SHORT);
	pad(id,'0',PAD_INT);
	pad(sactive,'0',PAD_BOOL);
	pad(ang,'0',PAD_FLOAT);
	pad(vel,'0',PAD_FLOAT);
	pad(x,'0',PAD_FLOAT);
	pad(y,'0',PAD_FLOAT);
	
	proj = type+display+id+sactive+ang+vel+x+y;
	
	return proj;
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
			TBAGame->generatePacket(EVENT_PROJ_COLLIDE,this->serialize());
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
					//Do not embed arrows into characters
					this->x = testX - (((rand()%10)+10)/100.0f)*(1+this->velocity)*std::cos(this->angle);
					this->y = testY - (((rand()%10)+10)/100.0f)*(1+this->velocity)*std::sin(this->angle);
					this->trackSubject = occupant;
					this->active = false;
					TBAGame->generatePacket(EVENT_PROJ_COLLIDE,this->serialize());
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
			this->location->objects.push_back(this);
		}
	}
	this->x = testX;
	this->y = testY;
}

void Projectile::cleanup() {
	this->location->removeObject(this);
	TBAGame->removeObject(this);
	delete this;
}
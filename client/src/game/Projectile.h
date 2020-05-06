#pragma once

#include "GameObject.h"
#include "../common/Tile.h"
#include "../common/Common.h"

#include <math.h>

class Projectile: public GameObject {
	public:
		float x,y;
		float damage;
		float angle;
		float velocity;
		bool active;
		int displaySize = 10;
		float collisionSize = .35; //.35 Meters in any direction
		int maxAge = TBAGame->convert(30000);	//Projectiles decay after 30 seconds

		int getDisplayID() {return 7;}
		
		std::string inline getName() {return "Arrow";}
		std::string inline getInfo() {return "Name:\t"+this->getName();}

		Tile* location;
		
		void updateLocation() {
			if((char*)TBAGame->gameWorld->getTileAt(this->x,this->y) != (char*)this->location) {
				this->location->removeObject(this);
				this->location = TBAGame->gameWorld->getTileAt(this->x,this->y);
				this->location->objects.push_back(this);
			}
		}

		GameObject* owner;
		GameObject* trackSubject = nullptr;

		Projectile(GameObject*, std::tuple<float,float>,float,float);

		unsigned int lastUpdate;
		unsigned int destroyTime;

		void update();

		std::tuple<float,float> getLocation() {
			return {this->x,this->y};
		}

		void cleanup();

		void relocate();
};
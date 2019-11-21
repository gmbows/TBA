#pragma once

#include <SDL2/SDL.h>

struct Tile;

#include "../common/Tile.h"
#include "Behavior.h"
#include "Inventory.h"
#include "Equipment.h"
#include "Statistics.h"
#include "GameObject.h"

#include <string>
#include <tuple>
#include <map>

typedef unsigned int flag;

class GameObject;

enum statusIndicator: flag {
	STATUS_IDLE = 		1 << 0,
	STATUS_MOVE = 		1 << 1,
	STATUS_ATTACK = 	1 << 2,
	STATUS_COMBAT =		1 << 3,
	STATUS_PURSUE = 	1 << 4,
	STATUS_STUN = 		1 << 5,
	STATUS_ESCAPE = 	1 << 6,
	STATUS_PATROL = 	1 << 7,
	STATUS_DYING = 		1 << 8,
	STATUS_CRIPPLED = 	1 << 9,
	STATUS_DEAD = 		1 << 10,
	STATUS_END = 		1 << 11,
};

inline statusIndicator operator|(statusIndicator f1,statusIndicator f2) {
	return (statusIndicator)((flag)f1 | (flag)f2);
}
inline statusIndicator operator&(statusIndicator f1,statusIndicator f2) {
	return (statusIndicator)((flag)f1 & (flag)f2);
}
inline bool operator==(statusIndicator f1,statusIndicator f2) {
	return (f1 ^ f2) == 0;
}
inline bool operator!=(statusIndicator f1,statusIndicator f2) {
	return (f1 ^ f2) > 0;
}


enum attackStatus: int {
	ATK_NOT_READY,
	ATK_STATUS_ATTACKING,
	ATK_COMPLETE,
};

class Character: public GameObject {

	public:

		//=============
		//		BASIC
		//=============

		float x,y;
		float ang = 0;
		bool move_forward = false;
		bool move_back = false;
		bool isPlayer;
		Inventory* inventory;
		std::string name;
		Tile* location;

		int displayID;
		int objectID;

		std::tuple<float,float> getAbsoluteLocation();
		std::tuple<float,float> getApproximateLocation();

		//=============
		//	  INVENTORY
		//=============

		std::string getInvString();
		

		//=============
		//	  EQUIPMENT
		//=============

		Equipment *equipment;
		std::string inline getEquipString() {return this->equipment->getEquipString();}
		bool equip(Item*);

		//=============
		//	  	STATS
		//=============

		StatSet *stats;

		//width in % of tile (one tile is 2x2 meters)
		float width = .25;

		void init_stats();

		unsigned int defaultAttackRate;

		//Unarmed damage
		int defaultAttackDamage;

		//Unarmed striking speed
		unsigned int defaultAttackSpeed;

		int maxHealth;
		int health;

		//In format 
		float defaultAttackRange;
		int defaultMoveSpeed;

		unsigned int attackRate;
		bool isUnarmed();
		int getAttackDamage();
		float getAttackRange();

		int lastAttack = 0;
		int lastMove = 0;

		//========
		//STATUS_MOVEMENT
		//========

		//In format units per ms
		float velocity = 0;
		int maxMoveSpeed;

		float traction;
		std::tuple<int,int> direction;

		void setLocation(float,float);
		void setLocomotion();
		void move();
		bool resolveMove(float&, float&);
		void moveTo(Character*);
		void moveAway(std::tuple<float,float>);

		//========
		//BEHAVIOR
		//========

		GameObject *target;

		//swapped to flags instead of single status int
		flag status = 1;
		
		//Target
		bool inline hasTarget() {return !(this->target == nullptr);}
		Character* getCharTarget();
		GameObject* getTarget();
		void setTarget(GameObject*);
		std::string getTargetInfo();
		std::string getTargetName();
		bool targetInRange();
		bool combatRetarget();
		bool getNearestTarget();
		bool findTargetInRadius(const std::string &name);
		GameObject* findObjectInRadius(const std::string &name);
		std::vector<Character*> getCharactersInRadius();
		std::vector<GameObject*> getObjectsInRadius(objType);
		inline Character* closer(Character* c1,Character* c2) {
			return (dist(this->getLocation(),c1->getLocation()) < dist(this->getLocation(),c2->getLocation()))? c1 : c2;

		}
		void lookAt(Character*);

		//Status
		void setStatus(statusIndicator);
		void addStatus(statusIndicator);

		inline void removeStatus(statusIndicator _s) {
			this->status = (this->status & ~_s);
			if(this->status == 0) this->addStatus(STATUS_IDLE);
		}

		inline flag getStatus() { return this->status; }
		inline bool hasStatus(statusIndicator _s) { return (this->status & _s) > 0;}
		bool inline isAlive() {return (this->health > 0);}
		std::string getStatusString();

		//Combat
		attackStatus getAttackStatus();
		void combat();
		void sendAttack(GameObject*);
		void receiveAttack(int, GameObject*);
		void resetCombatTimer();
		itemType getAttackType();

		//Kill
		void kill();

		//Misc
		void say(const std::string&);

		//==========
		//FOUNDATION
		//==========

		//Constructor and update/delete
		Character(bool,int,const std::string&,float=0,float=0);
		void update();
		virtual void cleanup();

		//Gameobject compliance functions
		virtual inline std::string getName() {return this->name;}
		virtual std::string getInfo();
		virtual inline int getID() {return this->objectID;}
		std::tuple<float,float> getLocation() {return {this->x,this->y}; }

		~Character() {
			delete this->equipment;
			delete this->inventory;
			//this->location->evict(this);
			delete this;
		}

};

#pragma once

#include <SDL2/SDL.h>

struct Tile;

#include "../common/Tile.h"
#include "Behavior.h"
#include "Inventory.h"
#include "Equipment.h"
#include "Statistics.h"
#include "GameObject.h"
#include "StatusEffect.h"
#include "ItemManifest.h"

#include "Limb.h"

#include <string>
#include <tuple>
#include <map>

typedef unsigned long int flag;

class GameObject;

enum statusIndicator: flag {
	STATUS_IDLE = 			1 << 0,
	STATUS_TRAVEL = 		1 << 1,
	STATUS_ATTACK = 		1 << 2,
	STATUS_NO_AMMO = 	1 << 11,
	STATUS_COMBAT =		1 << 3,
	STATUS_PURSUE = 		1 << 4,
	STATUS_STUN = 			1 << 5,
	STATUS_ESCAPE = 		1 << 6,
	STATUS_PATROL = 		1 << 7,
	STATUS_DYING = 		1 << 8,
	STATUS_CRIPPLED = 	1 << 9,
	STATUS_DEAD = 			1 << 10,
	STATUS_WORK = 		1 << 12,
	STATUS_END = 			1 << 13,
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
		//Aim angle in degrees
		float viewAng = 0;
		float targetAng = 0;
		
		float targetX = 0;
		float targetY = 0;
		
		Uint32 lastPathCheck = 0;
		
		std::vector<Tile*> targetPath;

		float getTurnSpeed() {return this->turnSpeed;}

		//Movement control
		bool move_forward = false;
		bool move_back = false;
		bool autoMove = false;
		bool isPlayer;
		Inventory* inventory;
		std::string name;
		Tile* location;

		int displayID;
		// int objectID;

		std::tuple<float,float> getAbsoluteLocation();
		std::tuple<float,float> getApproximateLocation();
		
		//=============
		//	  		LIMBS
		//=============
		
		std::vector<Limb*> limbs = {
			new Limb(100,LIMB_HEAD),
			new Limb(100,LIMB_TORSO),
			new Limb(100,LIMB_ARMS),
			new Limb(100,LIMB_LEGS),
		};
		
		//Checks limb HP and assigns statuses accordingly
		void checkLimbs();

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
		bool plant(Item*);
		bool consume(Item*);
		bool work(GameObject*);
		bool hasAmmo(ItemType);
		Item* getAmmo(ItemType);
		
		//=============
		//	  	STATUS EFFECTS
		//=============
		
		//for adding effects from an item
		void triggerItemEffects(Item*,Action);
		//for processing effects each tick
		void processEffects();
		std::vector<StatusEffect*> effects;
		
		bool hasEffects() {
			int e = 0;
			for(int i=0;i<this->effects.size();i++) {
				if(this->effects.at(i)->duration > 0) e++;
			}
			return e>0;
		}

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

		//In meters
		float defaultAttackRange;

		//In squares per tick
		int defaultMoveSpeed;
		int defaultTurnSpeed;
		int turnSpeed;
		int defaultFOV;

		unsigned int attackRate;
		bool isUnarmed();
		int getAttackDamage();
		float getAttackRange();

		int lastAttack = 0;
		int lastMove = 0;

		//========
		//STATUS_TRAVEL
		//========

		//In format units per ms
		float velocity = 0;
		float maxMoveSpeed;

		float traction;

		void setLocation(float,float);
		void setLocomotion();
		void move();
		//if viewang != targetang, attenuate at turnspeed
		void turn();
		void resolveMove(float&, float&);
		void moveToCharacter(Character*);
		bool generatePathTo(float,float,bool adjacent = false);
		void generatePathTo(Character*);
		void moveTo();
		void moveAway(Character*);
		
		bool inline hasPath() {return this->targetPath.size() > 0;}

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
		void setTargetAngle(Character*);
		void setTargetLoc(int,int);
		
		GameObject* workTarget = nullptr;
		bool inline hasWorkTarget() { return this->workTarget != nullptr;}
		void processWork();

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
		void exitCombat();
		void sendAttack(GameObject*);
		void receiveAttack(int, GameObject*);
		void resetCombatTimer();
		ItemType getAttackType();

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
		virtual inline std::string getFormattedName() {return '\x01'+("g"+this->name)+'\x01';}
		virtual inline std::string getName() {return this->name;}
		virtual std::string getInfo();
		virtual inline int getID() {return this->objectID;}
		std::tuple<float,float> getLocation() {return {this->x,this->y}; }

		~Character() {
			debug("Deleting character");
			delete this->equipment;
			delete this->inventory;
			//this->location->evict(this);
			delete this;
			debug("Done deleting character");
		}

};

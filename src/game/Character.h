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

enum statusIndicator: int {
	IDLE,
	MOVE,
	ATTACK,
	COMBAT,
	PURSUE,
	STUN,
	ESCAPE,
	PATROL,
	DYING,
	CRIPPLED,
	DEAD,
};

enum attackStatus: int {
	ATK_NOT_READY,
	ATK_ATTACKING,
	ATK_COMPLETE,
};

class Character: public GameObject {

	public:

		//=============
		//		BASIC
		//=============

		float x,y;
		bool isPlayer;
		Inventory* inventory;
		std::string name;
		Tile* location;

		int displayID;
		int objectID;

		std::tuple<float,float> getAbsoluteLocation();
		std::tuple<float,float> getApproximateLocation();

		//=============
		//	  EQUIPMENT
		//=============

		Equipment *equipment;

		//=============
		//	  	STATS
		//=============

		StatSet *stats;

		//Rates in format ms per action
		const int defaultAttackRate = 5000;

		//Unarmed damage
		const int defaultAttackDamage = 3;

		//Unarmed striking speed
		const int defaultAttackSpeed = 1000;

		int maxHealth = 10;
		int health;

		//In format 
		const float defaultAttackRange = 1*.5;
		const int defaultMoveSpeed = 1000 / 10;

		int attackRate;
		bool isUnarmed();
		int getAttackDamage();
		float getAttackRange();

		Uint32 lastAttack = 0;
		Uint32 lastMove = 0;

		//========
		//MOVEMENT
		//========

		//In format units per ms
		int velocityX = 0;
		int velocityY = 0;
		int maxMoveSpeed;

		float traction;
		std::tuple<int,int> direction;

		void setLocation(float,float);
		void setLocomotion();
		void move(std::tuple<int,int>);
		bool resolveMove(float&, float&);
		void moveTo(std::tuple<float,float>);
		void moveAway(std::tuple<float,float>);

		//========
		//BEHAVIOR
		//========

		GameObject *target;
		statusIndicator status;
		
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

		//Status
		void setStatus(statusIndicator);
		statusIndicator getStatus();
		bool inline isAlive() {return (this->health > 0);}

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

		~Character() {}

};

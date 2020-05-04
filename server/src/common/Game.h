#pragma once

#include "../tools/Log.h"
#include "../tools/Network.h"
#include "../tools/Utility.h"

#include "../../../shared/Shared.h"

#include "../game/GameObject.h"
#include "../game/World.h"
#include "../game/Character.h"
#include "../game/Command.h"
#include "Clock.h"

#include <thread>
#include <vector>
#include <queue>
#include <string>
#include <pthread.h>

struct Squad;

struct Client {

	std::string packet;

	int characterID;
	
	//Commands entered since last query
	std::queue<std::string> pendingCommands;
	//Any information that is controlled without using commands (client side)
	// Presently only location
	std::vector<int> info;
	
	//	00000000|00000000|00000000|c1|c2|c3|c4|...
 
	bool connected = false;
	bool hasNewPacket = false;
	
	void deserialize();
	
	Client(int id = -1) {
		if(id == -1) {
			// Character* newChar = new Character("New Client",64,{0,0});
			// this->characterID = newChar->objectID;
		}
	}
};

struct Game {

	//=============
	//	  BASIC
	//=============

	bool gameRunning;
	bool paused = false;
	bool debugMode = false;

	Log *gameLog;
	Clock *clock = new Clock();

	World* gameWorld;
	Character* playerChar;
	
	void setPlayer(Character*);

	//Global movespeed scale to translate from int movespeeds to floats
	const float moveSpeedUnit = .005;

	void displayText(const std::string &s) { 
		debug(this->gameLog->get_timestamp()+": "+s);
	}
	
	const char colorKey = '\x01';

	std::vector<std::string> getItemNames(const std::vector<Item*>&);
	
	//=============
	//		NETWORK
	//=============
	
	int maxClients = 5;
	Server *server;
	
	ClientThreadSpool *workers = new ClientThreadSpool(maxClients);

	std::map<int,Client*> clients;
	
	void processClientUpdates(Client*);
	
	std::string serializeObjects() {
		std::string objects = DATA_BEGIN;
		
		std::string type = std::to_string((int)DATA_OBJS);
		pad(type,'0',2);
		objects += type;
		
		for(int i=0;i<this->gameObjects.size();i++) {
			objects += this->gameObjects.at(i)->serialize();
		}
		objects += DATA_TERM;
		return objects;
	}

	//=======================
	//     GAME OBJECTS
	//=======================
	
	//The object that is displayed on the textscreen 
	//	when the player targets a character or object 
	
	GameObject* displayTarget = nullptr;
	bool inline hasDisplayTarget() { return !(this->displayTarget == nullptr);}
	void clearDisplayTarget() {this->displayTarget = nullptr;}
	void setDisplayTarget(GameObject *o) {this->displayTarget = o;}

	std::vector<GameObject*> convert(const std::vector<Character*>&);

	std::vector<GameObject*> gameObjects;
	std::vector<GameObject*> gameUIObjects;
	void updateGameObjects();
	void updateGameUIObjects();
	GameObject* findObject(int);
	GameObject* findObject(const std::string&);
	void removeObject(GameObject*);
	void removeUIObject(GameObject*);
	void removeObject(int);
	
	//==============
	//     	SQUADS
	//==============
	unsigned int numSquads;
	std::vector<Squad*> squads;
	Squad* createSquad(std::string s = "None");

	//To ensure unique object IDs
	int objectTotal = 0;
	
	//====================
	//	 UPDATE LOGIC
	//====================

	//Uint32 lastUpdate = SDL_GetTicks();
	Uint32 lastGraphicsUpdate;
	Uint32 lastLogicUpdate = 0;
	int timeToNextUpdate;
	int elapsed;
	int timeToNextLogicUpdate;
	int timeToNextGraphicsUpdate;

	//Amount of times game game window updates per second
	int graphicsTickRate = 60;

	//Amount of times game OBJECTS (NPC's, general game state) update per second
	int logicTickRate = 30;
	
	//Total ticks
	unsigned int logicTicks;
	unsigned int graphicsTicks;
	
	unsigned int inline convert(unsigned int ms) { return ms/(1000/this->logicTickRate);} 

	//recalculate npc pathing every 2 seconds at most
	int pathCheckInterval = convert(2000);

	//==========
	// COMMANDS
	//==========

	std::vector<Command*> commandList;
	std::vector<std::string> commandStrings;

	//====================
	// UPDATE / FOUNDATION
	//====================

	Game() {}
	~Game() {
		debug("Closing game");
		// delete this->gameWindow;
		this->gameObjects.clear();
		this->gameUIObjects.clear();
		std::vector<GameObject*>().swap(this->gameObjects);
		std::vector<GameObject*>().swap(this->gameUIObjects);
		// pthread_join(this->logic_thread,NULL);
		// pthread_join(this->graphics_thread,NULL);
	}

	void setupGame();

	bool togglePause();
	void popupText(int, const std::string&);
	
	void update();

	bool canUpdateLogic;
	bool canUpdateGraphics;

	void spawn_threads();

	void update_logic();

};
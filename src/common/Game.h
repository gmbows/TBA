#pragma once

#include "../ui/Window.h"
#include "../tools/Log.h"

#include "../game/GameObject.h"
#include "../game/World.h"
#include "../game/Character.h"
#include "../game/Command.h"
#include "Clock.h"

#include <thread>
#include <vector>
#include <string>
#include <pthread.h>

struct Game {

	//=============
	//	  BASIC
	//=============

	bool gameRunning;
	bool paused = false;
	bool debugMode = false;

	Window *gameWindow;
	Log *gameLog;
	Clock *clock = new Clock();

	World* gameWorld;
	Character* playerChar;
	
	void setPlayer(Character*);

	//Global movespeed scale to translate from int movespeeds to floats
	const float moveSpeedUnit = .005;

	void displayText(const std::string &s) { 
		this->gameWindow->textScreen->addContent(s);
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
	
	unsigned int inline convert(unsigned int ms) { return ms/this->logicTickRate;} 

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
		delete this->gameWindow;
		this->gameObjects.clear();
		this->gameUIObjects.clear();
		std::vector<GameObject*>().swap(this->gameObjects);
		std::vector<GameObject*>().swap(this->gameUIObjects);
		// pthread_join(this->logic_thread,NULL);
		// pthread_join(this->graphics_thread,NULL);
	}

	void setupUI();
	void setupGame();

	bool togglePause();
	void popupText(int, const std::string&);
	
	void input();
	void update();

	pthread_t logic_thread,graphics_thread;
	pthread_mutex_t updateLock;
	pthread_cond_t logic;
	pthread_cond_t graphics;

	bool canUpdateLogic;
	bool canUpdateGraphics;

	void spawn_threads();

	void update_logic();
	void update_graphics();

};

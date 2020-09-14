#pragma once

#include "../ui/Window.h"

#include "../tools/Log.h"

#include "../game/GameObject.h"
#include "../game/World.h"
#include "../game/Character.h"
#include "../game/Command.h"
#include "Clock.h"

#include "../tools/Algorithm.h"

#include <thread>
#include <vector>
#include <string>
#include <pthread.h>

struct Squad;

struct Game {

	//=============
	//	  BASIC
	//=============

	bool gameRunning;
	bool paused = false;
	bool debugMode = false;

	bool shift = false;

	bool m_forward = false;
	bool m_back = false;
	bool turn_right = false;
	bool turn_left = false;

	Window *gameWindow;
	Log *gameLog;
	Clock *clock = new Clock();

	World* gameWorld;
	Character* playerChar;
	
	void setPlayer(Character*);
	Character* getPlayer() {
		return this->playerChar;
	}

	//Global movespeed scale to translate from int movespeeds to floats
	const float moveSpeedUnit = .005;

	void displayText(const std::string &s) { 
		this->gameWindow->textScreen->addContent(s);
	}
	
	const char colorKey = '\x01';

	std::vector<std::string> getItemNames(const std::vector<Item*>&);
	
	//=============
	//		NETWORK
	//=============
	
	// Client *client = new Client();
	
	// std::string serializeInput();
	// void deserializeObjects(const std::string &s);
	
	// std::string objectUpdates;
	// bool needsUpdate = false;

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
	unsigned int objectTotal = 0;
	unsigned int itemTotal = 0;
	
	//==============
	//     	SQUADS
	//==============
	unsigned int numSquads;
	std::vector<Squad*> squads;
	Squad* createSquad(std::string s = "None");
	
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
	int pathCheckInterval = convert(1000);

	//==========
	// COMMANDS
	//==========

	std::vector<Command*> commandList;
	std::vector<std::string> commandStrings;

	//====================
	// UPDATE / FOUNDATION
	//====================

	Game() {
		this->graphicsTicks = 0;
		this->logicTicks = 0;
		this->logicEnabled = false;
		this->graphicsEnabled = false;
	}
	
	
	~Game() {
		debug("Closing game");
		delete this->gameWindow;
		// this->gameObjects.clear();
		// this->gameUIObjects.clear();
		std::vector<GameObject*>().swap(this->gameObjects);
		std::vector<GameObject*>().swap(this->gameUIObjects);
		// pthread_join(this->logic_thread,NULL);
		// pthread_join(this->graphics_thread,NULL);
	}

	void setupUI();
	void setupGame();
	
	//Rate (ms) at which the program polls keyboard and mouse input
	int input_interval = 36;
	
	//Performance stats
	
	int latency_notif_interval = 2000; //Interval at which update routines will notify that they are falling behind
	
	unsigned int last_perf_notif = 0;
	int perf_notif_interval = 250; //Interval at which update routines will notify that they are falling behind
	TBA_Interval perfNotifInterval = TBA_Interval(perf_notif_interval);
	
	//Size limited queues to keep track of last 100 updates times
	TBA_LimitedQueue<int> logicPerf = TBA_LimitedQueue<int>(100);
	TBA_LimitedQueue<int> graphicsPerf = TBA_LimitedQueue<int>(100);
	
	//Last elapsed update time for each update type
	int lastGraphicsUpdateTime;
	int lastLogicUpdateTime;

	bool togglePause();
	void popupText(int, const std::string&);
	
	void input();
	void update();
	
	//Threads
	pthread_t logic_thread;
	pthread_t graphics_thread;
	pthread_t input_thread;

	//Locks graphics updates until logic updates
	pthread_mutex_t graphicsLock = PTHREAD_MUTEX_INITIALIZER;
	
	//Signals graphics thread to proceed with update
	pthread_cond_t canUpdateGraphics = PTHREAD_COND_INITIALIZER;
	
	bool logicEnabled,graphicsEnabled;
	inline bool initialized() {return this->logicEnabled and this->graphicsEnabled;}

	void spawn_threads();

	void update_logic();
	void update_graphics();

};

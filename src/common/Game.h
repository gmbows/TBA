#pragma once

#include "../ui/Window.h"
#include "../tools/Log.h"

#include "../game/GameObject.h"
#include "../game/World.h"
#include "../game/Character.h"
#include "../game/Command.h"

#include <thread>
#include <vector>
#include <string>

struct Game {

	//=============
	//		BASIC
	//=============

	bool gameRunning;
	bool paused = false;

	Window *gameWindow;
	Log *gameLog;

	World* gameWorld;
	Character* playerChar;

	//Global movespeed scale to translate from int movespeeds to floats
	const float moveSpeedUnit = .00005;

	void displayText(const std::string &s) { 
		this->gameWindow->textScreen->addContent(s);
	}

	//=======================
	//			GAME OBJECTS
	//=======================
		
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
	//		  UPDATE LOGIC
	//====================

	Uint32 lastUpdate = SDL_GetTicks();
	Uint32 lastGraphicsUpdate;
	Uint32 lastLogicUpdate;
	int timeToNextUpdate;
	int elapsed;
	int timeToNextLogicUpdate;
	int timeToNextGraphicsUpdate;

	//Amount of times game game window updates per second
	int graphicsTickRate = 60;

	//Amount of times game OBJECTS (NPC's, general game state) update per second
	int logicTickRate = 50;
	
	//Total ticks
	int logicTicks;
	int graphicsTicks;

	//==========
	//	COMMANDS
	//==========

	std::vector<Command*> commandList;
	std::vector<std::string> commandStrings;	

	//====================
	//	UPDATE / FOUNDATION
	//====================

	Game() {}
	~Game() {
		debug("\nClosing game");
		delete this->gameWindow;
		this->gameObjects.clear();
		std::vector<GameObject*>().swap(this->gameObjects);

	}

	void setupUI();
	void setupGame();

	bool togglePause();
	void popupText(int, const std::string&);
	
	void input();
	void update();

};
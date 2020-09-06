#include <iostream>
#include <SDL2/SDL.h>
#include <pthread.h>

pthread_mutex_t printLock = PTHREAD_MUTEX_INITIALIZER;

#include "common/Common.h"


int main(int argc, char** argv) {
	// std::cout << "BLACK" << std::endl;
	// return 0;
	
	if(argc > 1) {
		TBAGame->debugMode = true;
		std::cout << "Debug mode enabled" << std::endl;
	}

	srand(time(NULL));

	SDL_Init(SDL_INIT_EVERYTHING);
	
	init_utility();
	
	//Initialize UI objects
	TBAGame->setupUI();

	//Initialized game objects
	TBAGame->setupGame();

	//defunct synchronization bools
	//TBAGame->canUpdateLogic = false;
	//TBAGame->canUpdateGraphics = true;

	//Start logic and graphics threads
	TBAGame->spawn_threads();

	//Let graphics thread update first
	// pthread_cond_signal(&TBAGame->graphics);
	
	// pthread_cond_wait(&TBAGame->canUpdateInput,&TBAGame->inputLock);
	
	TBAGame->gameLog->writeln("Entering game loop");
	while(TBAGame->gameRunning) {
		TBAGame->update();
	}

	delete TBAGame;
	SDL_Quit();
	
	return 0;
}

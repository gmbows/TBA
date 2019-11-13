#include <iostream>
#include <SDL2/SDL.h>
#include <pthread.h>

#include "common/Common.h"

using namespace std;

int main(int argc, char** argv) {

	srand(time(NULL));

	SDL_Init(SDL_INIT_EVERYTHING);
	
	//Initialize UI objects
	TBAGame->setupUI();

	//Initialized game objects
	TBAGame->setupGame();

	TBAGame->canUpdateLogic = false;
	TBAGame->canUpdateGraphics = true;

	//Let graphics thread update first
	TBAGame->spawn_threads();
	//pthread_cond_signal(&TBAGame->graphics);

	TBAGame->gameLog->writeln("Entering game loop");
	while(TBAGame->gameRunning) {
		TBAGame->input();
		TBAGame->update();
	}

	delete TBAGame;
	SDL_Quit();

	return 1;
}

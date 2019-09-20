#include <iostream>
#include <SDL2/SDL.h>

#include "common/Common.h"

using namespace std;

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_EVERYTHING);

	//Initialize UI objects
	TBAGame->setupUI();

	//Initialized game objects
	TBAGame->setupGame();

	TBAGame->gameLog->writeln("Entering game loop");
	while(TBAGame->gameRunning) {
		TBAGame->input();
		TBAGame->update();
	}

	delete TBAGame;
	SDL_Quit();

	return 1;
}
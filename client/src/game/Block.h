#pragma once

#include "GameObject.h"
#include <SDL2/SDL.h>

class Block: public GameObject {

	public:
		int id;

		SDL_RendererFlip flip;
		SDL_Texture* blockTexture;

		int rotation = 0;
		int flipType;
		bool randomized = false;
		bool passable;

		Block(int);

		void setFlipType(int f) {
			this->flipType = f;
			switch(this->flipType) {
				case 1:
					this->flip = SDL_FLIP_NONE;
					break;
				case 2:
					this->rotation = 0;
					this->flip = SDL_FLIP_HORIZONTAL;
					break;
				case 3:	
					this->flip = SDL_FLIP_VERTICAL;
					break;
				case 4:
					this->flip = SDL_FLIP_NONE;
					this->rotation = 180;
					break;
			}
		}
};
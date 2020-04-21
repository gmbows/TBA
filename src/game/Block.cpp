#include "Block.h"
#include "../common/Tile.h"
#include "../common/Common.h"
#include "../ui/Font.h"

Block::Block(int _id): id(_id), GameObject(OBJ_BLOCK) {
		int rotation = 0;
		bool randomized = false;
		if(tileMap.find(_id) == tileMap.end()) this->id = 1;
		this->flip = SDL_FLIP_NONE;
		
		this->blockTexture = SDL_CreateTexture(TBAGame->gameWindow->renderer,
                               SDL_GetWindowPixelFormat(TBAGame->gameWindow->window),
								SDL_TEXTUREACCESS_TARGET,
                               TBAGame->gameWindow->mapScreen->charW,
                               TBAGame->gameWindow->mapScreen->charH);

		fChar charInfo = TBAGame->gameWindow->mapScreen->screenFont->fontMap.at(this->id);

		SDL_Rect sRect = {charInfo.x,charInfo.y,charInfo.w,charInfo.h};

		SDL_SetRenderTarget(TBAGame->gameWindow->renderer,this->blockTexture);

		SDL_RenderCopyEx(TBAGame->gameWindow->renderer,TBAGame->gameWindow->mapScreen->screenFont->fontTexture,&sRect,NULL,0,NULL,SDL_FLIP_NONE);

		SDL_SetRenderTarget(TBAGame->gameWindow->renderer,NULL);

		this->passable = std::get<1>(tileMap.at(_id));
}
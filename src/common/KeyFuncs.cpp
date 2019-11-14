#include "KeyFuncs.h"

#include "../common/Common.h"
#include "../game/Behavior.h"
#include "../game/Projectile.h"
#include "../game/FloatingText.h"
#include "../tools/Utility.h"
#include "../game/Input.h"

#include <SDL2/SDL.h>
#include <tuple>

bool moving = false;
int activeScreen = 1;
int lastMove[2];

void processKeystroke(int keycode) {
	if(moving) {
		switch(keycode) {
			case 114:
				resetScreen(activeScreen);
				moving = false;
				return;
			case 82: 
				resetAllScreens();
				moving = false;
				return;
		}
		
	}
	char c = keycode;
	TBAGame->gameWindow->textScreen->commandAppend(c);
}

//==========
//  COMMAND
//==========

void sendCommand() {
	TBAGame->gameWindow->textScreen->sendCommand();
}

void swapCommand(int i) {
	TBAGame->gameWindow->textScreen->swapCommand(i);
}

void delChar() {
	//Pop back removes last entry in data structure
	TBAGame->gameWindow->textScreen->deleteLastCharacter();
}

//==========
//	  MACROS
//==========

void autocomplete() {
	if(autocomplete(TBAGame->gameWindow->textScreen->command,TBAGame->commandStrings)) {
		TBAGame->gameWindow->textScreen->commandAppend(' ');
	} else {
		//TBAGame->gameWindow->textScreen->setCommand(TBAGame->gameWindow->textScreen->command);
		TBAGame->gameWindow->textScreen->setCommandLine();
	}
}

void pause() {
	
	TBAGame->togglePause();

}

void clear() {
	TBAGame->gameWindow->textScreen->content.clear();
}

void clearCommand() {
	TBAGame->gameWindow->textScreen->command = "";
}

void paste() {
	std::string clipboard(SDL_GetClipboardText());
	TBAGame->gameWindow->textScreen->commandAppend(clipboard);
}

//==========
//	  MOUSE
//==========

void click(SDL_MouseButtonEvent& event) {


	//Left click
	int centerX = TBAGame->gameWindow->mapScreen->x+(TBAGame->gameWindow->mapScreen->w/2)-(TBAGame->gameWindow->mapScreen->charW*(TBAGame->playerChar->x));
	int centerY = TBAGame->gameWindow->mapScreen->y+(TBAGame->gameWindow->mapScreen->h/2)-(TBAGame->gameWindow->mapScreen->charH*(TBAGame->playerChar->y));

	int tileX = std::round(((float)(event.x-centerX))/TBAGame->gameWindow->mapScreen->charW);
	int tileY = std::round(((float)(event.y-centerY))/TBAGame->gameWindow->mapScreen->charH);

	Tile* thisTile = TBAGame->gameWorld->getTileAt(tileX,tileY);

	if(event.button == SDL_BUTTON_LEFT) {
		for(int i=TBAGame->gameWindow->screenVector.size()-1;i>=0;i--) {
			if(TBAGame->gameWindow->screenVector.at(i)->enclose(event.x,event.y)) {
				moving=true;
				activeScreen = TBAGame->gameWindow->screenVector.size()-1;
				TBAGame->gameWindow->bringToFront(TBAGame->gameWindow->screenVector.at(i));
				lastMove[0] = event.x;
				lastMove[1] = event.y;
				return;
			}
		}
		
		pthread_mutex_lock(&TBAGame->updateLock);
		if(thisTile->hasBlocks()) {
			thisTile->blocks.clear();
		} else {
			thisTile->addBlock(4);
		}
		pthread_mutex_unlock(&TBAGame->updateLock);

	} else if(TBAGame->gameWindow->mapScreen->enclose(event.x,event.y)) {

		GameObject* testTarget = nullptr;

		if(thisTile->isOccupied()) {
			testTarget = thisTile->occupiers.at(0);
			if((char*)TBAGame->displayTarget == (char*)testTarget) {
				testTarget = static_cast<GameObject*>(thisTile->getNextOccupant(static_cast<Character*>(testTarget)));
			}
		} else if(thisTile->hasObjects()) {
			testTarget = thisTile->objects.at(0);
		}
		TBAGame->displayTarget = testTarget;
	}

}

void release() {
	moving = false;
}

//==========
//	  SCREEN
//==========

void move(SDL_Event& event) {

	if(moving) {
		TBAGame->gameWindow->screenVector.at(activeScreen)->x += event.motion.x-lastMove[0];
		TBAGame->gameWindow->screenVector.at(activeScreen)->y += event.motion.y-lastMove[1];
	}
	lastMove[0] = event.motion.x;
	lastMove[1] = event.motion.y;
}

int getTopScreen(int x,int y) {
	int topScreenID = -1;
	for(int i=0;i<TBAGame->gameWindow->screenVector.size();i++) {
		if(TBAGame->gameWindow->screenVector.at(i)->enclose(x,y)) {
			topScreenID = i;
		}
	}
	return topScreenID;
}

void shiftContentWindow(int i) {

	//Get the topmost screen that the mouse is hovering over and scroll it
	int topScreenID = getTopScreen(lastMove[0],lastMove[1]);
	if(topScreenID < 0) return;
	TBAGame->gameWindow->screenVector.at(topScreenID)->shiftContentWindow(i);

}

void addContent(const std::string& s) {
	TBAGame->gameWindow->textScreen->addContent(s);
}

void resetScreen(int screenPriority) {
	TBAGame->gameWindow->screenVector.at(screenPriority)->x = TBAGame->gameWindow->screenVector.at(screenPriority)->defaultX;
	TBAGame->gameWindow->screenVector.at(screenPriority)->y = TBAGame->gameWindow->screenVector.at(screenPriority)->defaultY;
}

void resetAllScreens() {
	int screenCount = TBAGame->gameWindow->screenVector.size();
	for(int i=0;i<screenCount;i++) {
		resetScreen(i);
	}
}

//====================
//		PLAYER STATUS_MOVEMENT
//====================

void move(bool m_up,bool m_down,bool m_left, bool m_right) {

		std::tuple<int,int> Qdirection;

		int x = 0;
		int y = 0;

		if(m_up)	{
			y += 1;
		}
		if(m_down) 	{
			y -= 1;
		}
		if(m_left) 	{
			x -= 1;
		}
		if(m_right) {
			x =+ 1;
		}

	if(x ^ y) {
		if(TBAGame->playerChar->isAlive()) {
			TBAGame->playerChar->addStatus(STATUS_MOVE);
		}
	}

	if(TBAGame->playerChar->isAlive()) {
		Qdirection = std::make_tuple(x,y);
		TBAGame->playerChar->direction = Qdirection;
	}
}

//==========
//		MISC
//==========
int alpha = -1;

void debugKey() {

	//SDL_Delay(100);

	//Delete oldest object
	/*TBAGame->gameObjects.at(0)->cleanup();
	TBAGame->gameObjects.erase(TBAGame->gameObjects.begin());
	TBAGame->gameObjects.shrink_to_fit();*/

	//TBAGame->popupText(1,"Building is too close to existing settlement");


	//TBAGame->playerChar->setStatus(STATUS_ATTACK);

	//Check zoom
	//TBAGame->gameWindow->mapScreen->charW-=2;
	//TBAGame->gameWindow->mapScreen->charH-=2;

	//Increase movespeed
	//TBAGame->playerChar->maxMoveSpeed++;
	//debug(TBAGame->playerChar->maxMoveSpeed);

	//Simulate physics push
	//TBAGame->playerChar->move({200,150});

	//TBAGame->playerChar->isPlayer = false;
	//TBAGame->playerChar = static_cast<Character*>(TBAGame->gameObjects.at(1));
	//TBAGame->playerChar->isPlayer = true;

	// for(int i=0;i<10;i++) {
	// }

	//debug(TBAGame->gameObjects.size());
	//FloatingText* text = new FloatingText(1,{"TES"},{200,200});

	//TBAGame->playerChar->target->receiveAttack(0,TBAGame->playerChar);

	float x,y;
	decompose(TBAGame->playerChar->getLocation(),x,y);

	if(!TBAGame->playerChar->hasTarget()) {
		new Projectile(TBAGame->playerChar,{x,y},(-15+rand()%31),.02);
	} else {
		float tx,ty;
		decompose(TBAGame->playerChar->target->getLocation(),tx,ty);
		new Projectile(TBAGame->playerChar,{x,y},(-15+rand()%31)-atan2(ty-y,tx-x)/(3.1415/180),.02);
	}

	/*SDL_Texture* blockTexture = TBAGame->gameWorld->getTileAt(-2,-2)->blocks.at(TBAGame->gameWorld->getTileAt(-2,-2)->blocks.size()-1)->blockTexture;
	
	SDL_SetRenderTarget(TBAGame->gameWindow->renderer,blockTexture);

	fChar charInfo = TBAGame->gameWindow->mapScreen->screenFont->fontMap.at(TBAGame->gameWorld->getTileAt(-2,-2)->id);
	SDL_Rect sRect = {charInfo.x,charInfo.y,charInfo.w,charInfo.h};
	SDL_Rect dRect = {-3,-3,6,16};

	SDL_RenderCopyEx(TBAGame->gameWindow->renderer,TBAGame->gameWindow->mapScreen->screenFont->fontTexture,&sRect,&dRect,45,NULL,SDL_FLIP_NONE);

	SDL_SetRenderTarget(TBAGame->gameWindow->renderer,NULL);

	//TBAGame->graphicsTickRate = (++alpha%2 == 0)? 60 : 144;

	/*(std::vector<Tile*> tiles = TBAGame->gameWorld->getTilesInRadius(std::round(TBAGame->playerChar->x),std::round(TBAGame->playerChar->y),5);
	for(int i=0;i<tiles.size();i++) {
		tiles.at(i)->id = -1;
	}*/

	//SDL_SetTextureAlphaMod(TBAGame->gameWindow->mapScreen->screenFont->fontTexture,--alpha);
	//SDL_SetTextureColorMod(TBAGame->gameWindow->mapScreen->screenFont->fontTexture,255,alpha/2,alpha);
		

}

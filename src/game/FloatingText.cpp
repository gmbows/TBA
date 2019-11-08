#include "FloatingText.h"

#include "../common/Common.h"

void FloatingText::drawContent() {

	std::string thisLine;

	//Source rectangle taken from screenFont->fontTexture
	SDL_Rect sRect;
	fChar charInfo;

	//Destination rectangle on screen
	SDL_Rect dRect;

	//Cursor
	int cursor[2] = {0,0};

	int charIndex;

	//Center text above tracker
	int x = std::get<0>(this->location) - (TBAGame->gameWindow->textScreen->charW*this->message.at(0).size()/2);
	int y = std::get<1>(this->location) - (TBAGame->gameWindow->textScreen->charH*this->message.size()/2);
	
	//For each line of visible Content
	for(int i=0;i<this->message.size();i++) {
		thisLine = this->message.at(i);
		for(int j=0;j<thisLine.size();j++) {

			//Get character ASCII code and retrieve display values from font map file

			charIndex = int(thisLine[j]);

			if(TBAGame->gameWindow->textScreen->screenFont->fontMap.find(charIndex) == TBAGame->gameWindow->textScreen->screenFont->fontMap.end()) {
				debug("ERROR: Missing charMap entry for character "+thisLine[j]);
				exit(0);
			}

			charInfo = TBAGame->gameWindow->textScreen->screenFont->fontMap.at(charIndex);

			//Read values from charInfo into SDL_Rect
			sRect = {charInfo.x,charInfo.y,charInfo.w,charInfo.h};

			//Modify display location rectangle based on cursor and font display values << FIX
			SDL_Rect dRect = {x+(1*cursor[0])+this->offsetX,(TBAGame->gameWindow->textScreen->charH*cursor[1])+y+charInfo.yo-this->offsetY-this->dialogOffset,sRect.w,sRect.h};

			//SDL_RenderDrawRect(TBAGame->gameWindow->renderer,&dRect);
			SDL_RenderCopy(TBAGame->gameWindow->renderer,TBAGame->gameWindow->textScreen->screenFont->fontTexture,&sRect,&dRect);
		
			//Advance cursor for next character
			cursor[0] += charInfo.adv;
		}

		//Advance cursor to next line
		cursor[1]++;
		cursor[0] = 0;
	}
}

void FloatingText::update() {

	if(this->hasExpired()) {
		TBAGame->removeUIObject(this);
		this->trackSubject = nullptr;
		delete this;
		return;
	}

	//Check if floating text was created outside of screen view
	//if(std::abs(this->defaultX) > std::abs(TBAGame->playerChar->x)+TBAGame->gameWindow->mapScreen->screenCharWidth or std::abs(this->defaultY) > std::abs(TBAGame->playerChar->y)+TBAGame->gameWindow->mapScreen->screenCharHeight) {
	if(false) {
			TBAGame->removeUIObject(this);
			this->trackSubject = nullptr;
			delete this;
			return;
		}

	// Percent of duration completed
	this->completion = (TBAGame->logicTicks-this->created)/(this->duration*(float)TBAGame->convert(1000.0));
	//Dialog tracks characters and doesn't bubble
	if(this->tracking) {
		this->location = this->trackSubject->getAbsoluteLocation();
	} else {
		//Damage numbers stay in location where they were created and bubble
		//Dialog is always white with alpha 255
		SDL_SetTextureColorMod(TBAGame->gameWindow->textScreen->screenFont->fontTexture,this->color.r,this->color.g,this->color.b);
		this->offsetY = this->heightReached*this->completion;
		this->offsetX = this->offsetXsign*this->offsetXMult*this->completion;
		this->location = {
			TBAGame->gameWindow->mapScreen->x+(TBAGame->gameWindow->mapScreen->w/2)-(TBAGame->gameWindow->mapScreen->charW*(TBAGame->playerChar->x - this->defaultX)),
			TBAGame->gameWindow->mapScreen->y+(TBAGame->gameWindow->mapScreen->h/2)-(TBAGame->gameWindow->mapScreen->charH*(TBAGame->playerChar->y - this->defaultY))
		};
	}
	//SDL_SetTextureAlphaMod(TBAGame->gameWindow->textScreen->screenFont->fontTexture,200);
	this->drawContent();
	//Reset text screen font to default
	SDL_SetTextureColorMod(TBAGame->gameWindow->textScreen->screenFont->fontTexture,255,255,255);
	//SDL_SetTextureAlphaMod(TBAGame->gameWindow->textScreen->screenFont->fontTexture,200);
}

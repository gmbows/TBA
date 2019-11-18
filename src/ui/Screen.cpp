#include "Screen.h"
#include "Font.h"
#include "../common/Common.h"
#include "../tools/Utility.h"
#include "../common/Tile.h"

#include <SDL2/SDL.h>
#include <vector>
#include <map>
#include <random>
#include <ctime>

Screen::Screen(int x, int y, int w, int h, bool border) {

	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;

	this->defaultX = x;
	this->defaultY = y;

	this->hasBorder = border;

	screenFont = new Font("terminal");

	this->bdRect = {this->x,this->y,this->w,this->h};

}

TextScreen::TextScreen(int x, int y, int w, int h, bool border) {

	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;

	this->charW = 8;
	this->charH = 12;

	this->offsetX = 4;
	this->offsetY = 3;

	this->defaultX = x;
	this->defaultY = y;
	this->updatePriority = 0;

	this->hasBorder = border;
	this->screenCharWidth = this->w/this->charW;
	this->screenCharHeight = this->h/this->charH;

	this->tabChar = "->";
	this->commandHistory.push_back("");
	this->setCommandLine();

	screenFont = new Font("terminal");

}

TextBox::TextBox(int x, int y, int w, int h, bool border) {

	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;

	this->charW = 8;
	this->charH = 12;

	this->offsetX = 4;
	this->offsetY = 3;

	this->defaultX = x;
	this->defaultY = y;

	this->hasBorder = border;
	this->screenCharWidth = this->w/this->charW;
	this->screenCharHeight = this->h/this->charH;

	this->content.push_back("");

	screenFont = new Font("terminal");

	this->bdRect = {this->x,this->y,this->w,this->h};

	this->updatePriority = 1;

}

DynamicTextBox::DynamicTextBox(const std::string &content,int x, int y) {

	this->x = x;
	this->y = y;

	this->hasBorder = true;

	this->charW = 8;
	this->charH = 12;	
	this->w = std::min(this->maxWidth*charW,(this->charW*(int)content.size()) + (this->borderSize*2));
	this->screenCharWidth = this->w/this->charW;

	this->h = (this->charH*(content.size()/this->screenCharWidth)) + 6;
	this->screenCharHeight = this->h/this->charH;

	this->x -= this->w/2;
	this->y += this->h/2;

	this->offsetX = 5;
	this->offsetY = 3;

	this->defaultX = x;
	this->defaultY = y;

	this->setContent(content);

	screenFont = new Font("terminal");

	this->bdRect = {this->x,this->y,this->w,this->h};

}

MapScreen::MapScreen(int x, int y, int w, int h, bool border) {

	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;

	this->charW = 16;
	this->charH = 16;

	this->defaultX = x;
	this->defaultY = y;
	//this->updatePriority = 0;

	this->hasBorder = border;
	this->screenCharWidth = this->w/this->charW;
	this->screenCharHeight = this->h/this->charH;
	
	screenFont = new Font("map_tileset");
	
	this->bdRect = {this->x,this->y,this->w,this->h};
	this->mapTextureRect = {this->x+1,this->y+1,this->w+(2*this->charW),this->h+(2*this->charH)};

}

void Screen::update() {

	this->drawBorder();

}

void Screen::drawBorder() {

	if(this->hasBorder) {

		this->bdRect = {this->x,this->y,this->w,this->h};

		//Set drawing color to background color and draw background rectangle
		SDL_SetRenderDrawColor(TBAGame->gameWindow->renderer,bgColor.r,bgColor.g,bgColor.b,bgColor.a);
		SDL_RenderFillRect(TBAGame->gameWindow->renderer,&bdRect);
		//SDL_RenderCopy(TBAGame->gameWindow->renderer,this->screenFont->fontTexture,NULL,&bdRect);

		//Set drawing color to border color
		SDL_SetRenderDrawColor(TBAGame->gameWindow->renderer,bdColor.r,bdColor.g,bdColor.b,bdColor.a);

		for(int i=0;i<2;i++) {
			//Draw 3 pixel thick border
			SDL_Rect tRect = {bdRect.x-i,bdRect.y-i,bdRect.w+(2*i)+1,bdRect.h+(2*i)+1};
			SDL_RenderDrawRect(TBAGame->gameWindow->renderer,&tRect);
			//Pixels are painted at the end of the update function upon exiting
		}
	}
}

void Screen::drawContent(const std::vector<std::string>& screenContent) {

	//Source rectangle taken from screenFont->fontTexture
	SDL_Rect sRect;
	fChar charInfo;

	//Destination rectangle on screen
	SDL_Rect dRect;

	//Font color
	SDL_SetRenderDrawColor(TBAGame->gameWindow->renderer,255,255,255,255);

	int charIndex;

	//Format raw content string for display
	this->contentWindowStart = -this->screenCharHeight;
	this->contentWindowEnd = screenContent.size();

	if(this->contentWindowEnd+this->contentWindowOffset < this->screenCharHeight) {
		while(this->contentWindowEnd+this->contentWindowOffset < this->screenCharHeight) {
			this->contentWindowOffset++;
		} 
	} else if(this->contentWindowStart+this->contentWindowOffset > -this->screenCharHeight) {
		while(this->contentWindowStart+this->contentWindowOffset > -this->screenCharHeight) {
			this->contentWindowOffset--;
		}
	}
	//debug(this->contentWindowStart+this->contentWindowOffset);
	//debug(this->contentWindowEnd+this->contentWindowOffset);
	

	std::vector<std::string> visibleContent = subVec(screenContent,this->contentWindowStart+this->contentWindowOffset,this->contentWindowEnd+this->contentWindowOffset);

	//visibleContent.push_back("->");
	std::string thisLine;

	//Cursor
	int cursor[2] = {0,0};

	//For each line of visible Content
	for(int i=0;i<visibleContent.size();i++) {
		thisLine = visibleContent.at(i);
		for(int j=0;j<thisLine.size();j++) {

			//Get character ASCII code and retrieve display values from font map file
			charIndex = int(thisLine[j]);
			if(this->screenFont->fontMap.find(charIndex) == this->screenFont->fontMap.end()) {
				debug("ERROR: Missing charMap entry for character "+thisLine[j]);
				exit(0);
			}

			charInfo = this->screenFont->fontMap.at(charIndex);

			//Read values from charInfo into SDL_Rect
			sRect = {charInfo.x,charInfo.y,charInfo.w,charInfo.h};

			//Modify display location rectangle based on cursor and font display values << FIX
			SDL_Rect dRect = {this->offsetX+this->x+(1*cursor[0]),this->offsetY+(this->charH*cursor[1])+this->y+charInfo.yo,sRect.w,sRect.h};

			//SDL_RenderDrawRect(TBAGame->gameWindow->renderer,&dRect);
			SDL_RenderCopy(TBAGame->gameWindow->renderer,this->screenFont->fontTexture,&sRect,&dRect);
	
			//Advance cursor for next character
			cursor[0] += charInfo.adv;
		}

		//Advance cursor to next line
		cursor[1]++;
		cursor[0] = 0;
	}
}

void TextScreen::update() {

	this->drawBorder();
	this->setCommandLine();
	this->drawContent(extend(this->content,appendToLast(this->commandLines,this->cursorChar)));

}

void TextBox::update() {

	if(true or this->hasNewContent() or (this->lastUpdate + this->updateInterval <= SDL_GetTicks())) {
		this->drawBorder();
		//this->setContent("Inventory:"+TBAGame->playerChar->inventory->toString()+"\n\nGraphics Ticks: "+std::to_string(TBAGame->graphicsTicks)+"\nLogic Ticks: "+std::to_string(TBAGame->logicTicks)+"\nPlayer location: "+std::to_string((int)std::round(TBAGame->playerChar->x))+","+std::to_string((int)std::round(TBAGame->playerChar->y))+"\nPlayer velocity: "+std::to_string((int)std::max(std::abs(std::round(TBAGame->playerChar->velocityX)),std::abs(std::round(TBAGame->playerChar->velocityY))))+" MPH");	
		this->prepareContent();
		//this->setContent("Inventory:"+TBAGame->playerChar->inventory->contentString+"\n\nPlayer info:\n\t"+TBAGame->playerChar->getInfo()+"\n\nTarget info:\n\t"+TBAGame->playerChar->getTargetInfo());
		this->drawContent(this->content);
		this->lastUpdate = SDL_GetTicks();
	}
}

void DynamicTextBox::setToggledContent(const std::string& message) {

	this->fallbackText = message;
	if(this->messageQueue.size() == 0) {
		this->setContent(message);
	}

}

void DynamicTextBox::addMessage(int duration, const std::string& message) {

	this->enabled = true;
	this->messageQueue.push_back(std::make_tuple(message,duration));
	if(this->messageQueue.size() == 1) {
		this->setContent(message);
		this->stopTick = TBAGame->logicTicks+duration;
	}

}

void DynamicTextBox::update() {

	if(this->enabled and TBAGame->logicTicks >= this->stopTick) {
		this->messageQueue.erase(this->messageQueue.begin());
		if(this->messageQueue.size() == 0) {
			this->enabled = false;
			if(this->toggled) {
				this->setContent(this->fallbackText);
			}
		} else {
			this->setContent(std::get<0>(this->messageQueue.at(0)));
			this->stopTick = TBAGame->logicTicks+std::get<1>(this->messageQueue.at(0));
		}
	}

	this->drawBorder();
	this->drawContent(this->content);
}

void MapScreen::drawMap() {

	//Text offsets
	int offsetX = 0;//-this->charW;
	int offsetY = 0;//-this->charH;

	float centerX = this->lastMapX + .5;
	float centerY = this->lastMapY + .5;

	// this->mapTextureRect = {-playerOffsetX+offsetX+this->x,-playerOffsetY+offsetY+this->y,this->w+(2*this->charW),this->h+(2*this->charH)};
	this->mapTextureRect = {offsetX+this->x,offsetY+this->y,this->w,this->h};
	
	int windowOffsetX = ((TBAGame->gameWorld->size/2)+TBAGame->playerChar->x)*this->charW;
	int windowOffsetY = ((TBAGame->gameWorld->size/2)+TBAGame->playerChar->y)*this->charH;
	
	SDL_Rect srect = {windowOffsetX-(this->w/2),windowOffsetY-(this->h/2),this->w,this->h};
	
	SDL_RenderCopy(TBAGame->gameWindow->renderer,TBAGame->gameWorld->worldTexture,&srect,&this->mapTextureRect);

}

void MapScreen::trueDrawBorder() {

	//Drawborder without filling screen
	//For use after map screen has been drawn

	if(this->hasBorder) {

		this->bdRect = {this->x,this->y,this->w,this->h};

		//Set drawing color to background color and draw background rectangle
		//SDL_SetRenderDrawColor(TBAGame->gameWindow->renderer,bgColor.r,bgColor.g,bgColor.b,bgColor.a);
		//SDL_RenderFillRect(TBAGame->gameWindow->renderer,&bdRect);
		//SDL_RenderCopy(TBAGame->gameWindow->renderer,this->screenFont->fontTexture,NULL,&bdRect);

		//Set drawing color to border color
		SDL_SetRenderDrawColor(TBAGame->gameWindow->renderer,bdColor.r,bdColor.g,bdColor.b,bdColor.a);

		for(int i=0;i<2;i++) {
			//Draw 3 pixel thick border
			SDL_Rect tRect = {bdRect.x-i,bdRect.y-i,bdRect.w+(2*i)+1,bdRect.h+(2*i)+1};
			SDL_RenderDrawRect(TBAGame->gameWindow->renderer,&tRect);
			//Pixels are painted at the end of the update function upon exiting
		}
	}
}

void MapScreen::update() {

	this->drawBorder();
	this->updateMap();
	this->drawMap();
	this->trueDrawBorder();

}

void Panel::update() {

	SDL_Rect rect;
	SDL_Colour bgColor = TBAGame->gameWindow->bgColor;

	SDL_SetRenderDrawColor(TBAGame->gameWindow->renderer,bgColor.r,bgColor.g,bgColor.b,bgColor.a);

	for(int i=0;i<this->panelVector.size();i++) {
		rect = panelVector.at(i);
		SDL_RenderFillRect(TBAGame->gameWindow->renderer,&rect);
	}

}

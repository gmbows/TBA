#include "Window.h"
#include "../common/Common.h"
#include "../tools/Utility.h"

#include <iostream>
#include <SDL2/SDL.h>
#include <vector>

#include <thread>
#include <chrono>

Window::Window(int w,int h) {
	windowLog->writeln("New window initialized");

	this->window = SDL_CreateWindow("TBA_Game",
													SDL_WINDOWPOS_UNDEFINED,
													SDL_WINDOWPOS_UNDEFINED,
													w,h,
													SDL_WINDOW_OPENGL
													);

	this->renderer = SDL_CreateRenderer(this->window,-1,SDL_RENDERER_ACCELERATED);

	this->width = w;
	this->height = h;

	//SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);

	//std::string path = "assets/icon64.png";

	SDL_Surface *loadedSurface;
	SDL_RWops *rwop;
	rwop = SDL_RWFromFile("assets/icon/icon64.png","rb");
	loadedSurface = IMG_LoadPNG_RW(rwop);

	SDL_SetWindowIcon(window,loadedSurface);
	SDL_FreeSurface(loadedSurface);

}

void Window::generateScreenFontTextures() {

	this->mapScreen->screenFont->generateFontTexture(this->window,this->renderer);
	this->popupBox->screenFont->generateFontTexture(this->window,this->renderer);

	SDL_SetTextureAlphaMod(this->mapScreen->screenFont->fontTexture,255);

	for(int i=0;i<this->screenVector.size();i++) {
		screenVector.at(i)->screenFont->generateFontTexture(this->window,this->renderer);
	}	
}

void Window::drawScreens() {

	for(int i=0;i<this->screenVector.size();i++) {
		screenVector.at(i)->update();
	}	

}

void Window::bringToFront(Screen* screen) {
	//Advance screen through update priorities until it is at the front
	while(screen->updatePriority != this->screenVector.size()-1) {//2) {
		swap(this->screenVector,screen->updatePriority,screen->updatePriority+1);
		int oldPriority = screen->updatePriority;
		screen->updatePriority++;
		this->screenVector.at(oldPriority)->updatePriority = oldPriority;
	}
}

void Window::update(bool gdebug) {

	SDL_SetRenderDrawColor(this->renderer,bgColor.r,bgColor.g,bgColor.b,bgColor.a);
	SDL_RenderClear(this->renderer);

	if(gdebug == false) this->mapScreen->update();
	TBAGame->updateGameUIObjects();
	//this->mapPanel->update();
	if(this->popupBox->enabled or this->popupBox->toggled) {
		this->popupBox->update();
	}
	
	//Main drawing functions go here
	//-------------------
	this->drawScreens();
	//-------------------

	//TBAGame->updateGameUIObjects();

	//DEBUG MAP CENTER
	// SDL_SetRenderDrawColor(this->renderer,255,255,255,255);
	// SDL_RenderDrawPoint(this->renderer,this->mapScreen->x+this->mapScreen->w/2,this->mapScreen->y+this->mapScreen->h/2);

	SDL_RenderPresent(this->renderer);

}

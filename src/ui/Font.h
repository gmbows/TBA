#pragma once

#include "../tools/Log.h"
#include "../tools/Utility.h"

#include <iostream>
#include <fstream>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

//A struct to assign display values from font map file to individual characters
struct fChar { int i,x,y,w,h,xo,yo,adv; };

struct Font {

	Log *fontLog = new Log("Font.txt");
	
	//Path to font directory + fontName
	std::string basePath;

	//Font filename without file type
	std::string fontName;

	//SDL Texture containing all characters and glyphs in font
	SDL_Texture *fontTexture;
	
	//A map to assign char ASCII values to display values from font map
	std::map<int,fChar> fontMap;

	Font(const std::string& fontname) {
		this->basePath = "assets/font/"+fontname;
		this->fontName = fontname;
		this->loadFontMap();
	}

	~Font() {
		SDL_DestroyTexture(this->fontTexture);
		delete this->fontLog;
	}

	void generateFontTexture(SDL_Window*,SDL_Renderer*);
	void loadFontMap();

};

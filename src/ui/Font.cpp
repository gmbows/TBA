#include "Font.h"
#include "../tools/Utility.h"

#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void Font::generateFontTexture(SDL_Window *window, SDL_Renderer *renderer) {

	//Font
	std::string path = "assets/font/"+this->fontName+"_0.tga";

	fontLog->writeln("Opening front from path "+path);
	SDL_Surface* optimizedSurface = NULL;
	SDL_Surface *loadedSurface;
	SDL_RWops *rwop = SDL_RWFromFile(path.c_str(),"rb");
	loadedSurface = IMG_LoadTGA_RW(rwop);
	//Check if font opened properly..?

	fontLog->writeln("Optimizing font surface...");
	optimizedSurface = SDL_ConvertSurface( loadedSurface, SDL_GetWindowSurface(window)->format, 0 );
	fontLog->writeln("Font surface optimized");

	SDL_FreeSurface( loadedSurface );


	//Color keying
	//Everything on loaded texture of color 0,0,0 will be renderered transparent
	SDL_SetColorKey(optimizedSurface, SDL_TRUE, SDL_MapRGB( optimizedSurface->format, 0, 0, 0 ) );

	fontTexture = SDL_CreateTextureFromSurface(renderer,optimizedSurface);

	SDL_SetTextureBlendMode(fontTexture,SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(fontTexture,200);

	fontLog->writeln("Font texture generation complete");
	//SDL_SetTextureColorMod(glyphText,210,210,210);

}

void Font::loadFontMap() {

	fontLog->writeln("Loading font map...");

	std::ifstream fontMapFile;
	fontMapFile.open("assets/font/"+fontName+".fnt");

	std::string line;
	std::string str_i,str_x,str_y,str_w,str_h,str_xoff,str_yoff,str_adv;

	fChar c;
	int i;

	while(std::getline(fontMapFile,line)) {

			//Grab font display values from font map file
			str_i = line.substr(8,5);
			str_x = line.substr(15,5);
			str_y = line.substr(23,6);
			str_w = line.substr(35,6);
			str_h = line.substr(48,6);
			str_xoff = line.substr(62,6);
			str_yoff = line.substr(76,6);
			str_adv = line.substr(91,6);

			//Insert font display values into fChar struct to be retrieved at time of display
			i = std::stoi(str_i);
			c.x = std::stoi(str_x);
			c.y = std::stoi(str_y);
			c.w = std::stoi(str_w);
			c.h = std::stoi(str_h);
			c.xo = std::stoi(str_xoff);
			c.yo = std::stoi(str_yoff);
			c.adv = std::stoi(str_adv);
						
			fontMap.insert(std::pair<int,fChar>(i,c));
			
	}

	fontLog->writeln("Font map loaded.");
}


#pragma once

#include "../tools/Log.h"
#include "../tools/StringFuncs.h"
#include "Screen.h"

#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <string>

class Window {

	public:

		//SDL Window and Renderer objects
		SDL_Window *window;
		SDL_Renderer *renderer;

		TextScreen *textScreen;
		MapScreen *mapScreen;
		TextBox *auxScreen;
		Panel *mapPanel;
		DynamicTextBox *popupBox;

		//Vector containing all screen objects for easy updates
		std::vector<Screen*> screenVector;

		//Height and width of game window
		int width,height;

		//Default background color
		SDL_Colour bgColor = {13,13,13,255};

		//Window log
		Log *windowLog = new Log("Window.txt");

		//Generate font textures for each screen in screenVector
		void generateScreenFontTextures();

		//Constructor + Destructor
		Window(int,int);
		~Window() {
			SDL_DestroyRenderer(this->renderer);
			SDL_DestroyWindow(this->window);
			delete textScreen;
			delete mapScreen;
			delete mapPanel;
			delete auxScreen;
			delete popupBox;
			delete this->windowLog;
		}

		//Basic screen functions
		void update();
		void input();
		void drawScreens();
		void bringToFront(Screen*);
};


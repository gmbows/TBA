#pragma once

#include "../tools/Log.h"
#include "../tools/Utility.h"
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
		
		std::vector<DynamicTextBox*> popupVector;
		void deleteFirstToggledPopup();

		void createPopup(const std::string&,int duration,bool toggled);

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
			debug("Cleaning up game window...");
			SDL_DestroyRenderer(this->renderer);
			SDL_DestroyWindow(this->window);
			delete this->textScreen;
			delete this->mapScreen;
			// delete this->mapPanel;
			// delete this->auxScreen;
			delete this->windowLog;
			debug("Done cleaning up game window.");
		}

		//Basic screen functions
		void update(bool);
		void input();
		void drawScreens();
		void bringToFront(Screen*);
};


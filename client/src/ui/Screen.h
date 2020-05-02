#pragma once

#include "Font.h"
#include "../common/Tile.h"
#include "../game/GameObject.h"

#include <SDL2/SDL.h>
#include <string>
#include <utility>
#include <unordered_set>


class Screen {

	public:
		int defaultX, defaultY;
		int x,y,h,w;
		bool hasBorder;

		SDL_Texture *screenTexture;
		void init_texture();

		//Rectangle representing screen boundaries
		SDL_Rect bdRect;

		//Border color
		SDL_Colour bdColor = {155,155,155,255};
	
		//Background color
		SDL_Colour bgColor = {2,2,2,255};

		//Font
		Font *screenFont;
		int charW;
		int charH;
		int screenCharWidth;
		int screenCharHeight;

		std::string cursorChar;

		int offsetX;
		int offsetY;

		//Update updatePriority (highest = top)
		int updatePriority;

		virtual void update();

		//Text content of screen
		std::vector<std::string> content;

		Screen(int,int,int,int,bool);
		Screen(){}
		~Screen() {
			debug("Deleting generic screen...");
			// delete this->screenFont;
			// SDL_DestroyTexture(this->screenTexture);
			debug("Done deleting generic screen.");
		}

		//Global content draw function (command screen is specialized in update override)
		void generateTexture(const std::vector<std::string>&);
		void drawScreen();
		void shiftContentWindow(int);

		//Global border draw function across all screen classes
		void drawBorder();

		int contentWindowStart,contentWindowEnd;
		int contentWindowOffset = 0;

		bool enclose(int,int);

};

class TextScreen: public Screen {

	public:

		TextScreen(int,int,int,int,bool);
		TextScreen(){}

		//Command line
		std::string command;
		std::string tabChar;
		std::string cursorChar = "_";
	
		std::vector<std::string> commandLines;
		std::vector<std::string> lastContent = {};

		std::vector<std::string> commandHistory;
		int commandIndex;

		void addCommand();
		void setCommand(const std::string&);
		void swapCommand(int);

		void setCommandLine();
		void addContent(const std::string&);
		void addContent(char);
		void setLastLine(const std::string&);
		void commandAppend(char);
		void commandAppend(const std::string&);
		void deleteLastCharacter();
		void resetContentWindowOffset();

		std::vector<std::string> prepareCommandForDisplay(const std::string&);

		void sendCommand();

		void update();

};

class TextBox: public TextScreen {

	public:

		std::string oldContent;

		TextBox(int,int,int,int,bool);
		TextBox() {}

		int lastUpdate = 0;
		int updateInterval = 10;

		void prepareContent();
		std::string getContent();
		bool inline hasNewContent() {return (this->getContent() != this->oldContent);}

		void setContent(const std::string&);

		void update();

};

class DynamicTextBox: public TextBox {

		//Same as a textbox except the size of the is assigned based on starting text

		public:

			bool active = true;
			bool toggled = false;

			std::string fallbackText;

			int duration;

			int borderSize = 4;
			int maxWidth = 60;
			int maxHeight = 10;

			std::vector<std::tuple<std::string,int>> messageQueue;

			void addMessage(int, const std::string&);
			void setToggledContent(const std::string&);

			DynamicTextBox(const std::string&,int,int,int);

			void setContent(const std::string&);

			void update();

};

class MapScreen: public Screen {

	public:

		std::vector<std::vector<Tile*>> map;

		//to keep track of redrawn tiles each loop
		std::unordered_set<char*> checked;
		

		SDL_Texture* mapTexture;
		SDL_Rect mapTextureRect;
	
		void drawMap();
		void trueDrawBorder();
		//void generateMapTexture();
		void updateMap();
		void redrawActiveTiles();
		void generateMapTiles();

		int zoom = 1;

		int lastMapX = 400;
		int lastMapY = 0;

		MapScreen(int,int,int,int,bool);
		
		~MapScreen() {
			debug("Deleting map screen...");
			// SDL_DestroyTexture(this->mapTexture);
			debug("Done deleting map screen.");
		}

		void update();

};

class Panel: public Screen {

	public:
		std::vector<SDL_Rect> panelVector;

		void update();

};

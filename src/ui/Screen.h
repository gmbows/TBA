#pragma once

#include "Font.h"
#include "../common/Tile.h"
#include "../game/GameObject.h"

#include <SDL2/SDL.h>
#include <string>
#include <utility>


class Screen {

	public:
		int defaultX, defaultY;
		int x,y,h,w;
		bool hasBorder;

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
			delete this->screenFont;
		}

		//Global content draw function (command screen is specialized in update override)
		void drawContent(const std::vector<std::string>&);
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
		std::vector<std::string> commandLines;

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
		void deleteLastCharacter();
		void resetContentWindowOffset();

		std::vector<std::string> prepareCommandForDisplay(const std::string&);

		void sendCommand();

		void update();

};

class TextBox: public TextScreen {

	public:

		TextBox(int,int,int,int,bool);
		TextBox() {}

		void prepareContent();

		void setContent(const std::string&);

		void update();

};

class DynamicTextBox: public TextBox {

		//Same as a textbox except the size of the is assigned based on starting text

		public:

			bool enabled = false;
			bool toggled = false;

			std::string fallbackText;

			int stopTick;

			int borderSize = 4;
			int maxWidth = 60;
			int maxHeight = 10;

			std::vector<std::tuple<std::string,int>> messageQueue;

			void addMessage(int, const std::string&);
			void setToggledContent(const std::string&);

			DynamicTextBox(const std::string&,int,int);

			void setContent(const std::string&);

			void update();

};

class MapScreen: public Screen {

	public:

		std::vector<std::vector<Tile*>> map;

		SDL_Texture* mapTexture;
		SDL_Rect mapTextureRect;
	
		void drawMap();
		void trueDrawBorder();
		void generateMapTexture();
		void generateMapTiles();

		int lastMapX = 400;
		int lastMapY = 0;

		MapScreen(int,int,int,int,bool);

		void update();

};

class Panel: public Screen {

	public:
		std::vector<SDL_Rect> panelVector;

		void update();

};

#pragma once

#include "Character.h"
#include "../common/Tile.h"
#include "Structure.h"

#include <vector>

class World {

	public:

		//Size of one quadrant of game world excluding axis
		int size;

		std::vector<std::vector<Tile*>*> tileVector;
		
		// SDL_Texture* worldTexture;

		Tile* invalid = new Tile(-1,-1,-1);
		
		World(int i): size(i) {}

		//Generates and inserts tiles into world
		void genWorld();

		std::vector<std::vector<Tile*>> getMapAt(int,int,int);
		Tile* getTileAt(float,float);
		bool locationIsValid(float,float);
		bool locationInBoundary(float,float);
		void cartesianToIndex(float&,float&);
		void indexToCartesian(int&,int&);

		void createStructure(std::tuple<int,int>, structure,int);
		void replaceTile(std::tuple<int,int>,int);

		std::vector<Tile*> getTilesInRadius(int,int,int);
		
		//====================
		// NETWORK
		//====================
		
		std::string serialize();
		
		~World() {
			this->tileVector.clear();
			delete this->invalid;
			std::vector<std::vector<Tile*>*>().swap(this->tileVector);
		}

};
#pragma once

#include "Character.h"
#include "../common/Tile.h"
#include "Structure.h"
#include "../ui/Font.h"

#include <vector>

class World {

	public:

		//Size of one quadrant of game world excluding axis
		int size;

		std::vector<std::vector<Tile*>*> tileVector;
		
		SDL_Texture* worldTexture;

		Tile* invalid = new Tile(-1,-1,-1);
		
		Font *screenFont;
		
		World(int i): size(i) {
			this->screenFont = new Font("map_tileset");
		}

		//Generates and inserts tiles into world
		void genWorld();
		void genWorld_new(SDL_Renderer*);

		std::vector<std::vector<Tile*>> getMapAt(int,int,int);
		Tile* getTileAt(float,float);
		bool locationIsValid(float,float);
		bool locationInBoundary(float,float);
		void cartesianToIndex(float&,float&);
		void indexToCartesian(int&,int&);
		
		//=============
		//    PATHING
		//=============
		bool hasSimplePath(GameObject* c1, GameObject* c2);
		std::vector<Tile*> simplePathTo(int,int,int,int);
		std::vector<Tile*> simplePathTo(GameObject* c1, GameObject* c2);
		bool validatePath(const std::vector<Tile*> &path);

		void createStructure(std::tuple<int,int>, structure,int);
		void replaceTile(std::tuple<int,int>,int);

		std::vector<Tile*> getTilesInRadius(int,int,int);
		
		~World() {
			this->tileVector.clear();
			delete this->invalid;
			std::vector<std::vector<Tile*>*>().swap(this->tileVector);
		}

};
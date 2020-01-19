#pragma once

class Character;

#include "../game/Character.h"
#include "../game/Block.h"

#include <iostream>
#include <SDL2/SDL.h>
#include <random>
#include <ctime>
#include <map>
#include <vector>

extern std::map<int,std::tuple<float,bool,bool>> tileMap;
extern std::map<int,std::string> tileNameMap;

struct Tile {
	
	int id;
	int x,y;
	int rotation;
	SDL_RendererFlip flip;
	
	bool passable = true;
	bool invalid = false;
	bool randomized = false;

	bool needsUpdate = false;
	
	bool highlight = false;
	
	//How fast character speed decays
	float roughness;

	std::string getName();

	//================
	//		OCCUPANTS
	//================
	
	bool inline isOccupied() {return this->occupiers.size() > 0;}
	std::vector<Character*> occupiers;
	void evict(Character*);
	Character* getNextOccupant(Character*);
	void occupyWith(Character*);

	//================
	//		  OBJECTS
	//================

	bool inline hasObjects() {return this->objects.size() > 0;}
	void addObject(GameObject* o) {this->objects.push_back(o);}
	void removeObject(GameObject*);
	std::vector<GameObject*> objects;

	//=======
	// BLOCKS
	//=======

	std::vector<Block*> blocks;
	bool inline hasBlocks() {return this->blocks.size() > 0;}
	void addBlock(int);
	void addBlock(Block*);

	//================
	//	  INFO RETRIEVAL
	//================

	int getRotation();
	SDL_RendererFlip getFlip();
	SDL_Texture* getBlockTexture();
	bool isPassable();
	int inline getDisplayID() {return ((this->blocks.size() == 0)? this->id : this->blocks.at(this->blocks.size()-1)->id); }

	Tile(int,int,int);
	

	~Tile() {
		this->occupiers.clear();
		this->objects.clear();
		this->blocks.clear();
		std::vector<Character*>().swap(this->occupiers);
		std::vector<GameObject*>().swap(this->objects);
		std::vector<Block*>().swap(this->blocks);
	}

	

};
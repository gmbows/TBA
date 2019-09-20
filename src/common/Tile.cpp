#include "Tile.h"

#include "../tools/StringFuncs.h"
#include "../game/Character.h"

#include <iostream>
#include <SDL2/SDL.h>
#include <random>
#include <ctime>
#include <vector>
#include <map>
								
std::map<int,std::tuple<float,bool,bool>> tileMap {

	//id,roughness,passable,randomized

	{-1,{1,false,false}},	//Invalid
	{0,{.04,true,true}},	//Grass
	{1,{.13,true,false}},	//Grass with rock
	{2,{.04,false,false}},	//Tree
	{3,{.04,true,false}},	//Player
	{4,{.04,false,false}},	//Stone brick
	{5,{.04,false,false}},	//Stone brick Bottom left
	{6,{.04,true,false}},	//NPC
	// {7,{.04,true,false}},	//Arrow
	{8,{.04,true,false}},	//Chest
	
};

Tile::Tile(int tid, int _x, int _y): id(tid), x(_x), y(_y) {

	if(tileMap.find(tid) == tileMap.end()) {
		debug("Invalid tileID, using generic");
		this->id = -1;
		tid = -1;
	}

	this->roughness = std::get<0>(tileMap.at(tid));
	this->passable = std::get<1>(tileMap.at(tid));
	this->randomized = std::get<2>(tileMap.at(tid));
	
	if(tid == -1) {
		this->invalid = true;
	}

	if(randomized) {
		this->rotation = 90*(rand()%4);
		switch(rand()%3) {
			case 0:
				this->flip = SDL_FLIP_NONE;
				break;
			case 1:
				this->flip = SDL_FLIP_HORIZONTAL;
				break;
			case 2:
				this->flip = SDL_FLIP_VERTICAL;
				break;
		}
	} else {
		this->rotation = 0;
		this->flip = SDL_FLIP_NONE;
	}
}

void Tile::occupyWith(Character* c) {
	this->occupiers.push_back(c);
}

void Tile::addBlock(int bid) {
	this->blocks.push_back(new Block(bid));
}

void Tile::addBlock(Block* block) {
	this->blocks.push_back(block);
}

void Tile::evict(Character* occupant) {
	int i = find(occupant, this->occupiers);

	if(i >= 0) {
		this->occupiers.erase(this->occupiers.begin() + i);
		if(this->occupiers.size() > 1) {
			this->occupiers.shrink_to_fit();
		}
	}
}

void Tile::removeObject(GameObject* generic) {
	int i = find(generic, this->objects);

	if(i >= 0) {
		this->objects.erase(this->objects.begin() + i);
		if(this->objects.size() > 1) {
			this->objects.shrink_to_fit();
		}
	}
}

Character* Tile::getNextOccupant(Character* c) {

	int charIndex = find(c,this->occupiers);

	if(charIndex < 0) {
		return c;
	}

	if(charIndex < this->occupiers.size()-1) {
		return this->occupiers.at(charIndex+1);
	} else {
		return this->occupiers.at(0);
	}

}

int Tile::getRotation() {
	if(this->hasBlocks()) {
		return this->blocks.at(this->blocks.size()-1)->rotation;
	}
	return this->rotation;
}

SDL_Texture* Tile::getBlockTexture() {
	if(this->hasBlocks()) {
		return this->blocks.at(this->blocks.size()-1)->blockTexture;
	}
	debug("RETURNING INVALID TEXTURE");
	return nullptr;
}

bool Tile::isPassable() {
	if(this->hasBlocks()) {
		return this->blocks.at(this->blocks.size()-1)->passable;
	} else if(this->hasObjects()) {
		for(int i=0;i<this->objects.size();i++) {
			if(this->objects.at(i)->type == OBJ_CONTAINER) {
				return false;
			}
		}
	}
	return this->passable;
}

SDL_RendererFlip Tile::getFlip() {
	if(this->hasBlocks()) {
		return this->blocks.at(this->blocks.size()-1)->flip;
	}
	return this->flip;
}
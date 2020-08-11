#pragma once
#include "../common/Tile.h"
#include "Utility.h"

#include <vector>

class Character; 

std::vector<Tile*> generatePath(Character*,float tx, float ty, bool adjacent);
bool characterCanReach(Character*,float,float,bool);

extern unsigned int node_count;
extern unsigned int trials;
extern unsigned int total_iter;

//heuristic function
float h(Tile *tile,Tile *start, Tile *end,float bias = .001);

struct Node {

	//Unique identifier
	int id;
	
	//Path info
	Tile* tile;
	Tile* start;
	Tile* end;
	
	//Bias
	//Values larger than default (.001) will allow for more exploration
	float default_bias = .001;
	float bias = .001;
	
	void setBias(float b) {
		this->bias = std::min(b,1.0f);
	}
	
	float gScore;
	float hScore;	
	
	inline float getX() {return this->tile->x;}
	inline float getY() {return -this->tile->y;}

	float f() {
		return h(this->tile,this->start,this->end,this->bias) + this->gScore;
	}
		
	Node(Tile* t,Tile* s, Tile* e): tile(t), start(s), end(e) {
		
		this->id = ++node_count;
		
		this->gScore = 0xFFFFFF;
		this->hScore = h(tile,start,end);
		
	}
	
	Node() {}
	
};

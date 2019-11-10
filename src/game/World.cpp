#include "World.h"
#include "Character.h"
#include "../common/Tile.h"
#include "Structure.h"
#include "../tools/Utility.h"

#include <vector>
#include <random>
#include <ctime>

int lightRound(float x) {

	return ( std::abs((int)(x*10)%10) >= 5 )? (int)( x + ((x > 0)? 1 : -1)) : (int)x;
	//y = ( std::abs((int)(y*10)%10) >= 5 )?  (int)( y + ((y > 0)? 1 : -1)) : (int)y;

}

void World::indexToCartesian(int &x,int &y) {

	//Convert world vector indices to cartesian locations used for game objects

	x -= this->size/2;
	y -= this->size/2;

}

void World::cartesianToIndex(float &x,float &y) {

	//Convert cartesian map coordinates used in character locations to indices in World tileVector

	//x = ( std::abs((int)(x*10)%10) >= 5 )? (int)( x + ((x > 0)? 1 : -1)) : (int)x;
	//y = ( std::abs((int)(y*10)%10) >= 5 )?  (int)( y + ((y > 0)? 1 : -1)) : (int)y;

	//x = ( (( ((int)x < 0)? -1 : 1 ) * (int)(x*10)%10) >= 5 )? (int)( x + ((x > 0)? 1 : -1)) : (int)x;
	//y = ( (( ((int)y < 0)? -1 : 1 ) * (int)(y*10)%10) >= 5 )?  (int)( y + ((y > 0)? 1 : -1)) : (int)y;

	//lightRound(x,y);

	x = std::round(x);
	y = std::round(y);

	//x = lightRound(x);
	//y = lightRound(y);

	x += this->size/2;
	y += this->size/2;

}

void World::genWorld() {

	srand(time(NULL));

	debug("Generating world!");

	int cartesianX;
	int cartesianY;

	for(int y=0;y<this->size;y++) {
		this->tileVector.push_back(new std::vector<Tile*>);
		for(int x=0;x<this->size;x++) {
			cartesianX = x;
			cartesianY = y;
			indexToCartesian(cartesianX,cartesianY);
			if(rand()%10 == 0) {
				this->tileVector.at(y)->push_back(new Tile(2,cartesianX,-cartesianY));
			} else if(rand()%10 == 1) {
				this->tileVector.at(y)->push_back(new Tile(1,cartesianX,-cartesianY));
			} else {
				this->tileVector.at(y)->push_back(new Tile(0,cartesianX,-cartesianY));
			}
		}
	}
}

void World::createStructure(std::tuple<int,int> location, structure s, int tileID) {

	//Takes tile at top left of structure and places tiles according to structure plan

	int startingX = std::get<0>(location) - (s.at(0).size()/2);
	int startingY = std::get<1>(location) - (s.size()/2);

	float indX,indY;

	Block* block;

	for(int i=0;i<s.size();i++) {
		for(int j=0;j<s.at(i).size();j++) {
			//Check corner types
			this->replaceTile({startingX+j,startingY+i},0);
			//if( (j == 0 and i == 0) or (j == 0 and i == s.size()-1) or (j == s.at(i).size()-1 and i == 0) or (j == s.at(i).size()-1 and i == s.size()-1) ) {
			if(s.at(i).at(j) < 0) {
				block = new Block(4);
				//block->setFlipType(-s.at(i).at(j));
				this->getTileAt(startingX+j,startingY+i)->addBlock(block);
				continue;
			}
			if(s.at(i).at(j) == 1) {
				this->getTileAt(startingX+j,startingY+i)->addBlock(new Block(tileID));
			}
		}
	}

}

//=========
//		TILES
//=========

void World::replaceTile(std::tuple<int,int> location, int tileID) {
	int x = std::get<0>(location);
	int y = std::get<1>(location);

	float indX = x;
	float indY = y;

	this->cartesianToIndex(indX,indY);
	
	delete this->getTileAt(x,y);
	this->tileVector.at(indY)->at(indX) = new Tile(tileID,x,-y);

}

std::vector<Tile*> World::getTilesInRadius(int x,int y,int r) {

	int rs = pow(r,2);

	std::vector<Tile*> inRadius;

	for(int i=y-r;i<=y+r;i++) {
		for(int j=x-r;j<=x+r;j++) {
			if(	pow(j-x,2) + pow(i-y,2) <= rs) {
				inRadius.push_back(getTileAt(j,i));
			}
		}
	}
	return inRadius;
}

//Takes Cartesian map data
std::vector<std::vector<Tile*>> World::getMapAt(int mapSize,int x,int y) {

	std::vector<std::vector<Tile*>> newMap;

	for(int i=0;i<mapSize;i++) {
		std::vector<Tile*> tileLine;
		newMap.push_back(tileLine);
		for(int j=0;j<mapSize;j++) {
			newMap.at(i).push_back(this->getTileAt(x-(mapSize/2)+j,y-(mapSize/2)+i));
		}
	}

	return newMap;

}

//Takes Cartesian location data and converts to Indexed before retrieving tile
Tile* World::getTileAt(float x,float y) {

	if(!locationIsValid(x,y)) {
		return this->invalid;
	}

	cartesianToIndex(x,y);

	return this->tileVector.at(y)->at(x);
}

//Takes Cartesian location data AND converts to indexed location data
bool World::locationIsValid(float x,float y) {

	cartesianToIndex(x,y);

	return !(x >= this->size or x < 0 or y >= this->size or y < 0);

}
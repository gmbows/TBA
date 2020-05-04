#pragma once
#include "GameObject.h"
#include "Inventory.h"
#include "../common/Tile.h"

#include "../common/Common.h"
#include "../tools/Utility.h"

#include <tuple>

class Container: public GameObject {

	public:

		float x,y;

		std::string name;
		Inventory* inventory;
		Tile* location;
		
		std::string serialize();

		std::tuple<float,float> inline getLocation() {return {this->x,this->y};}

		Container(const std::string&,std::tuple<float,float>, int);
		Container(const std::string&,std::tuple<float,float>, Inventory*);
		Container(const std::string&,std::tuple<float,float>, int, const std::vector<int>&);

		int getDisplayID() {return 8;}

		std::string inline getName() {return this->name;}
		std::string inline getFormattedName() {return "\x01g"+this->name+"\x01";}
		std::string getInfo();

		void cleanup() {
			debug("Deleting container");
			delete this->inventory;
			TBAGame->removeObject(this);
			delete this;
			debug("Done deleting container");
		}

};
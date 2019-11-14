#pragma once

#include "ItemManifest.h"

#include <vector>
#include <string>
#include <ctime>
#include <map>

class Item {

	public:
		int id;
		
		std::string name;
		int weight;
		int size;
		time_t created;

		flag types;
		itemType primaryType;
		//AttributeSet *attributes;
		std::map<itemAttribute,float> attributes;
		

		Item(int);

		inline flag getType() {return this->types;}
		inline bool hasType(itemType type) { return (this->types & type) > 0;}

		itemType getPrimaryType();

		bool hasAttribute(itemAttribute);
		float getAttribute(itemAttribute);
		std::vector<itemAttribute> getAttributes();
		void createAttributeSet(const std::vector<itemAttribute>&,const std::vector<float>&);


		std::string getName();
		std::string getPlural();
		std::string getPluralDisplayName();
		std::string getDisplayName();

		~Item() {
			//delete this->attributes;
		}

};

inline bool operator==(Item i1,Item i2) {
	return (char*)&i1 == (char*)&i2;
}
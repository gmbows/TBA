#pragma once

#include "ItemManifest.h"
#include "Attributes.h"

#include <vector>
#include <string>
#include <ctime>

class Item {

	private:
		//std::map<itemAttribute,int> attributeValues;

	public:
		int id;
		
		std::string name;
		int weight;
		int size;
		time_t created;

		std::vector<itemType> types;
		AttributeSet *attributes;
		itemType primaryType;

		Item(int);

		bool hasAttribute(itemAttribute);
		bool hasType(itemType);
		float getAttribute(itemAttribute);

		std::string getName();

		~Item() {
			delete this->attributes;
		}

};
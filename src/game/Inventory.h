#pragma once

#include "Item.h"
#include "../tools/StringFuncs.h"

#include <vector>
#include <string>

struct Inventory {

	int capacity;
	std::vector<Item*> *contents = new std::vector<Item*>;
	
	Inventory(int);
	void add(Item*);
	void add(int);
	void add(const std::vector<int>&);

	void setInfoString();
	std::string contentString;

	~Inventory() {
		this->contents->clear();
		std::vector<Item*>().swap(*this->contents);
		delete this->contents;
	}

};
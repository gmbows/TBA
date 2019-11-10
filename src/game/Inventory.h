#pragma once

#include "Item.h"
#include "../tools/Utility.h"

#include <vector>
#include <string>

struct Inventory {

	int capacity;
	std::vector<Item*> *contents = new std::vector<Item*>;
	
	Inventory(int);
	void add(Item*);
	void add(int);
	void add(const std::vector<int>&);

	inline Item* getItem(int i) {return this->contents->at(i);}

	int find(Item*);
	int find(const std::string&);

	std::vector<std::string> getContentString();

	Item* remove(int);

	void setInfoString();
	std::string contentString;

	~Inventory() {
		this->contents->clear();
		std::vector<Item*>().swap(*this->contents);
		delete this->contents;
	}

};
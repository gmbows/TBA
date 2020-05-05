#pragma once

#include "Item.h"
#include "../tools/Utility.h"

#include <vector>
#include <string>

struct Inventory {

	int capacity;
	std::vector<Item*> *contents = new std::vector<Item*>;
	
	std::string serialize();
	
	Inventory(int);
	bool add(int);
	bool add(Item*);
	bool add(const std::vector<int>&);
	bool add(const std::vector<Item*>&);
	int find(Item*);
	int find(const std::string&);
	
	int getFirstInstance(const std::string&);

	int itemCount(int);

	Item* remove(int);
	Item* remove(Item*);

	inline Item* getItem(int i) {return this->contents->at(i);}
	std::vector<Item*> getItemsOfType(ItemType);

	std::vector<std::string> getContentString();
	inline bool isEmpty() { return this->contents->size() == 0; } 
	
	void setInfoString();
	std::string contentString;
	std::string toString();

	~Inventory() {
		this->contents->clear();
		std::vector<Item*>().swap(*this->contents);
		delete this->contents;
	}

};
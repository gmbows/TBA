#pragma once

#include "Item.h"
#include "../tools/Error.h"
#include "../tools/Utility.h"

#include <vector>
#include <string>
#include <map>
#include <utility>

struct Inventory {

	int capacity;
	std::vector<Item*> *contents = new std::vector<Item*>;
	
	// void initializeManifest();
	std::map<int,unsigned int> manifest;
	void record(int,int);
	
	bool hasChanged = false;
	
	Inventory(int);
	bool add(int,int amt = 1);
	bool add(Item*);
	bool add(const std::vector<int>&);
	bool add(const std::vector<Item*>&);
	int find(Item*);
	int find(const std::string&);
	int find(int);
	Item* get(int id) {
		if(id < 0 or id >= this->contents->size()) {
			TBA_throw(WARN_RET_NULLPTR,__PRETTY_FUNCTION__);
			return nullptr;
		}
		return this->contents->at(id);
	}
	
	int getFirstInstance(const std::string&);

	int itemCount(int);

	Item* remove(int);
	Item* remove(Item*);

	inline Item* getItem(int i) {return this->contents->at(i);}
	std::vector<Item*> getItemsOfType(ItemType);

	std::vector<std::string> getContentString();
	inline bool isEmpty() { return this->contents->size() == 0; } 

	// void setInfoString();
	std::string getInfoString();
	std::string toString();
	// std::string contentString;

	~Inventory() {
		this->contents->clear();
		std::vector<Item*>().swap(*this->contents);
		delete this->contents;
	}

};
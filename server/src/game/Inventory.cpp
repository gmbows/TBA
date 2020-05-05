#include "Inventory.h"
#include "ItemManifest.h"
#include "Item.h"

#include "../../../shared/Shared.h"
#include "../tools/Utility.h"

Inventory::Inventory(int _capacity): capacity(_capacity) {
}

std::string Inventory::serialize() {
	std::string invsize = std::to_string(this->contents->size());
	pad(invsize,'0',PAD_INT);
	std::string s = invsize;
	for(int i=0;i<this->contents->size();i++) {
		s += this->contents->at(i)->serialize();
	}
	return s;	
}

//===========
// 		 ADD
//===========

bool Inventory::add(Item* item) {

	this->contents->push_back(item);
	this->setInfoString();
	return true;
}

bool Inventory::add(int id) {

	this->contents->push_back(new Item(id));
	this->setInfoString();
	return true;
}

bool Inventory::add(const std::vector<int>& itemVec) {

	for(int i=0;i<itemVec.size();i++) {
		this->contents->push_back(new Item(itemVec.at(i)));
	}
	this->setInfoString();
	return true;
}

bool Inventory::add(const std::vector<Item*>& itemVec) {

	for(int i=0;i<itemVec.size();i++) {
		this->contents->push_back(itemVec.at(i));
	}
	this->setInfoString();
	return true;
}



int Inventory::find(Item* item) {
	for(int i=0;i<this->contents->size();i++) {
		if(item == this->contents->at(i)) return i;
	}
	return -1;
}

int Inventory::find(const std::string &name) {
	std::vector<std::string> contentVec = this->getContentString();
	std::string itemName = name;
	if(!autocomplete(itemName,contentVec)) {
		//Autocomplete partially filled word,
		// special return code for interaction parsing
		if(itemName != name) return -2;
	}
	return this->getFirstInstance(itemName);
}

int Inventory::getFirstInstance(const std::string &itemName) {
		for(int i=0;i<this->contents->size();i++) {
			if(itemName == this->contents->at(i)->name) return i;
	}
	return -1;
}

int Inventory::itemCount(int id) {
	int c = 0;
	for(int i=0;i<this->contents->size();i++) {
		if(this->contents->at(i)->id == id) c++;
	}
	return c;
}

std::string Inventory::toString() {
	if(this->isEmpty()) return "\n\tEmpty";
	return this->contentString;
}

////////////////
//INTERACTION
//MUST SET INFO STRING
////////////////

std::vector<Item*> Inventory::getItemsOfType(ItemType type) {
	std::vector<Item*> compatible;
	for(int i=0;i<this->contents->size();i++) {
		if(this->contents->at(i)->hasType(type)) compatible.push_back(this->contents->at(i));
	}
	return compatible;
}

Item* Inventory::remove(int index) {
	Item *removedItem = this->getItem(index);
	this->contents->erase(this->contents->begin()+index);
	this->setInfoString();
	return removedItem;
}

Item* Inventory::remove(Item* item) {
	int index = this->find(item);
	if(index >= 0) {
		this->contents->erase(this->contents->begin()+index);
		this->setInfoString();
	}
	return item;
}

std::vector<std::string> Inventory::getContentString() {
	std::vector<std::string> contents;
	for(int i=0;i<this->contents->size();i++) {
		if(!contains(contents,this->contents->at(i)->name)) {
			contents.push_back(this->contents->at(i)->name);
		}
	}
	return contents;
}

void Inventory::setInfoString() {

	std::vector<int> idList;
	std::vector<int> checkedIDs;
	std::string invString = "\n";
	int itemID;
	int itemCount;
	std::string itemName;

	if(this->isEmpty()) {
		this->contentString = "\tEmpty";
		return;
	}

	//Populate idList with list of itemID's in inventory
	for(int i=0;i<this->contents->size();i++) {
		idList.push_back(this->contents->at(i)->id);
	}
	
	for(int i=0;i<this->contents->size();i++) {

		itemID = this->contents->at(i)->id;
		if(contains(checkedIDs,itemID)) {
			continue;
		}

		itemName = replace(this->contents->at(i)->name,' ',"");
		itemCount = count(idList,itemID);
		if(itemCount == 1) {
			invString += "\t-"+itemName;
		} else {
			// invString += "\t-"+std::to_string(itemCount)+""+this->contents->at(i)->getPluralDisplayName();
			invString += "\t-"+std::to_string(itemCount)+"x"+this->contents->at(i)->getDisplayName();
			//invString += "\t-"+itemName+""+"("+std::to_string(itemCount)+")";
		}
		checkedIDs.push_back(itemID);
	}

	this->contentString = invString;

}
#include "Inventory.h"
#include "ItemManifest.h"
#include "Item.h"

#include "../tools/Utility.h"

Inventory::Inventory(int _capacity): capacity(_capacity) {
}

//===========
// 	  MANIFEST
//===========

void Inventory::record(int id,int change) {
	if(this->manifest.find(id) == this->manifest.end()) {
		this->manifest.insert({id,1});
	} else {
		int temp = this->manifest.at(id);
		//Delete entry if we're removing the last item of this type
		this->manifest.erase(id);
		if(temp + change > 0) {
			this->manifest.insert({id,temp+change});
		}
	}
}


//===========
// 		 ADD
//===========

bool Inventory::add(Item* item) {

	this->contents->push_back(item);
	this->record(item->id,1);
	return true;
}

bool Inventory::add(int id,int amt) {
	Item *newItem;
	for(int i=0;i<amt;i++) {
		newItem = new Item(id);
		this->contents->push_back(newItem);
		this->record(id,1);
	}
	//delete newItem;
	return true;
}

bool Inventory::add(const std::vector<int>& itemVec) {
	Item *newItem;
	for(int i=0;i<itemVec.size();i++) {
		newItem = new Item(itemVec.at(i));
		this->contents->push_back(newItem);
		this->record(itemVec.at(i),1);
	}
	//delete newItem;
	return true;
}

bool Inventory::add(const std::vector<Item*>& itemVec) {
	for(int i=0;i<itemVec.size();i++) {
		this->contents->push_back(itemVec.at(i));
		this->record(itemVec.at(i)->id,1);
	}
	return true;
}

//===========
// 		REMOVE
//===========

Item* Inventory::remove(int index) {
	Item *removedItem = this->getItem(index);
	this->contents->erase(this->contents->begin()+index);
	this->record(removedItem->id,-1);
	return removedItem;
}

Item* Inventory::remove(Item* item) {
	int index = this->find(item);
	if(index >= 0) {
		this->contents->erase(this->contents->begin()+index);
		this->record(item->id,-1);
	}
	return item;
}

//===========
// 		 FIND
//===========

int Inventory::find(int id) {
	for(int i=0;i<this->contents->size();i++) {
		if(id == this->contents->at(i)->id) return i;
	}
	return -1;
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

//============
//		OTHER
//============
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
	return this->getInfoString();
}

std::vector<Item*> Inventory::getItemsOfType(ItemType type) {
	std::vector<Item*> compatible;
	for(int i=0;i<this->contents->size();i++) {
		if(this->contents->at(i)->hasType(type)) compatible.push_back(this->contents->at(i));
	}
	return compatible;
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

std::string Inventory::getInfoString() {
	
	std::string stuff = "\n";
	std::string name;
	int id,count;
	for(std::map<int,unsigned int>::iterator it=this->manifest.begin();it != this->manifest.end();it++) {
		id = it->first;
		count = this->manifest.at(id);
		
		name = Item(id).getDisplayName();
		
		if(count == 1) {
			stuff += "\t-"+name;
		} else {
			// invString += "\t-"+std::to_string(itemCount)+""+this->contents->at(i)->getPluralDisplayName();
			stuff += "\t-"+std::to_string(count)+"x"+name;
			//invString += "\t-"+itemName+""+"("+std::to_string(itemCount)+")";
		}
	}
	return stuff;
}


//===========
// 		 DEFUNCT
//===========

// void Inventory::setInfoString() {
	
	// this->hasChanged = true;

	// std::vector<int> idList;
	// std::vector<int> checkedIDs;
	// std::string invString = "\n";
	// int itemID;
	// int itemCount;
	// std::string itemName;

	// if(this->isEmpty()) {
		// this->contentString = "\tEmpty";
		// return;
	// }

	// Populate idList with list of itemID's in inventory
	// for(int i=0;i<this->contents->size();i++) {
		// idList.push_back(this->contents->at(i)->id);
	// }
	
	// for(int i=0;i<this->contents->size();i++) {

		// itemID = this->contents->at(i)->id;
		// if(contains(checkedIDs,itemID)) {
			// continue;
		// }

		// itemName = replace(this->contents->at(i)->name,' ',"");
		// itemCount = count(idList,itemID);
		// if(itemCount == 1) {
			// invString += "\t-"+itemName;
		// } else {
			// invString += "\t-"+std::to_string(itemCount)+""+this->contents->at(i)->getPluralDisplayName();
			// invString += "\t-"+std::to_string(itemCount)+"x"+this->contents->at(i)->getDisplayName();
			// invString += "\t-"+itemName+""+"("+std::to_string(itemCount)+")";
		// }
		// checkedIDs.push_back(itemID);
	// }

	// this->contentString = invString;

// }
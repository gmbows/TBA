#include "Inventory.h"
#include "ItemManifest.h"
#include "Item.h"

#include "../tools/StringFuncs.h"

Inventory::Inventory(int _capacity): capacity(_capacity) {
}

void Inventory::add(Item* item) {

	this->contents->push_back(item);
	this->setInfoString();
}

void Inventory::add(int id) {

	this->contents->push_back(new Item(id));
	this->setInfoString();
}

void Inventory::add(const std::vector<int>& itemVec) {

	for(int i=0;i<itemVec.size();i++) {
		this->contents->push_back(new Item(itemVec.at(i)));
	}
	this->setInfoString();
}

void Inventory::setInfoString() {

	std::vector<int> idList;
	std::vector<int> checkedIDs;
	std::string invString = "\n";
	int itemID;
	int itemCount;
	std::string itemName;
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
		invString += "\t-"+itemName+"("+std::to_string(itemCount)+")";
		checkedIDs.push_back(itemID);
	}

	this->contentString = invString;

}
#include "Item.h"
#include "ItemManifest.h"
#include "../tools/StringFuncs.h"

#include <map>
#include <tuple>
#include <ctime>

Item::Item(int _id): id(_id) {

	if(itemManifest.find(id) == itemManifest.end()) {
		debug("ERROR: Adding invalid item with id "+std::to_string(id)+", using invalid item");
	}

	itemTraits itemInfo = itemManifest.at(id);

	this->name = std::get<0>(itemInfo);
	this->weight = std::get<1>(itemInfo);
	this->size = std::get<2>(itemInfo);
	this->types = std::get<3>(itemInfo);
	this->primaryType = this->types.at(0);
	this->attributes = new AttributeSet(attributeLookup.at(this->primaryType),std::get<4>(itemInfo));

	/*std::vector<itemAttribute> attribEnums = attributeLookup.at(this->primaryType);
	std::vector<int> attribValues = std::get<4>(itemInfo);

	for(int i=0;i<attribEnums.size();i++) {
		this->attributeValues.insert({attribEnums.at(i),attribValues.at(i)});
	}
*/

	this->created = std::time(NULL);

}
 
bool Item::hasAttribute(itemAttribute attrib) {
	//return (this->attributeValues.find(attrib) != this->attributeValues.end());
	return this->attributes->hasAttribute(attrib);
}

float Item::getAttribute(itemAttribute attrib) {

	// Check that at least one of this item's types has
	// attribute attrib, then return value at attribute index
	//	in this item's attribute value vector

	/*if(this->hasAttribute(attrib)) {
		return this->attributeValues.at(attrib);
	}
	return -1;*/
	return this->attributes->getValue(attrib);

}

std::string Item::getName() {
	//For advanced item types with names that may be different than default
	return this->name;
}

bool Item::hasType(itemType type) {
	return contains(this->types,type);
}
#include "Item.h"
#include "ItemManifest.h"
#include "../tools/Utility.h"

#include <map>
#include <tuple>
#include <ctime>

Item::Item(int _id): id(_id) {

	if(id >= itemManifest.size()) {
		debug("ERROR: Adding invalid item with id "+std::to_string(id)+", using invalid item");
	}

	itemTraits itemInfo = itemManifest.at(id);

	this->name = std::get<0>(itemInfo);
	this->weight = std::get<1>(itemInfo);
	this->size = std::get<2>(itemInfo);
	this->types = std::get<3>(itemInfo);
	this->primaryType = this->getPrimaryType();
	this->createAttributeSet(this->getAttributes(),std::get<4>(itemInfo));

	/*std::vector<itemAttribute> attribEnums = attributeLookup.at(this->primaryType);
	std::vector<int> attribValues = std::get<4>(itemInfo);

	for(int i=0;i<attribEnums.size();i++) {
		this->attributeValues.insert({attribEnums.at(i),attribValues.at(i)});
	}
*/

	this->created = std::time(NULL);

}

std::vector<itemAttribute> Item::getAttributes() {
	//Returns vector of attributes that this item needs values for
	if(attributeLookup.find(this->primaryType) != attributeLookup.end()) {
		return attributeLookup.at(this->primaryType);
	} else {
		return {};
	}
}

itemType Item::getPrimaryType() {
	//Returns type that appears first in the itemType enum
	flag testFlag;
	for(int i=0;i<log2((flag)I_END);i++) {
		testFlag = 1 << i;
		if(this->hasType((itemType)testFlag)) return (itemType)testFlag;
	}
	//Should never return I_END
	// item should have received a type
	return I_END;
}

void Item::createAttributeSet(const std::vector<itemAttribute> &attribs,const std::vector<float> &attribValues) {

	// If list of attribute values received from item manifest
	// is longer than list of attributes from attribute lookup map,
	// trailing attributes and values will be ignored

	int attribCount = std::min(attribs.size(),attribValues.size());

	for(int i=0;i<attribCount;i++) {
		this->attributes.insert({attribs.at(i),attribValues.at(i)});
	}
}


bool Item::hasAttribute(itemAttribute attrib) {
	//return (this->attributeValues.find(attrib) != this->attributeValues.end());
	return (this->attributes.find(attrib) != this->attributes.end());
}

float Item::getAttribute(itemAttribute attrib) {

	if(this->hasAttribute(attrib)) {
		return this->attributes.at(attrib);
	}
	return -1;

}

std::string Item::getName() {
	//For advanced item types with names that may be different than default
	return this->name;
}

#include "Item.h"
#include "ItemManifest.h"
#include "../tools/Utility.h"

#include <map>
#include <tuple>
#include <ctime>

std::map<itemType,std::string> itemTypeMap = {
	{I_FOOD,"Food"},
	{I_FRUIT,"Fruit"},
	{I_VEGETABLE,"Vegetable"},
	{I_WEAPON,"Weapon"},
	{I_EQUIPMENT,"Equipment"},
	{I_WEAPON_MELEE,"Melee weapon"},
	{I_WEAPON_RANGED,"Ranged weapon"},
	{I_WEAPON_SWORD,"Sword weapon"},
	{I_WEAPON_BOW,"Bow weapon"},
	{I_ARMOR,"Armor"},	
	{I_ARMOR_HEAD,"Armor"},	
	{I_ARMOR_BODY,"Body armor"},	
	{I_ARMOR_LEGS,"Leg armor"},	
	{I_ARMOR_FEET,"Foot armor"},
	{I_CRAFTING,"Crafting item"},
	{I_METAL,"Metal"},
	{I_INGREDIENT,"Ingredient"},
	{I_CURRENCY,"Currency"},
	{I_CONTAINER,"Container"},
	
	
};

Item::Item(int _id): id(_id) {

	if(id >= itemManifest.size()) {
		debug("ERROR: Adding invalid item with id "+std::to_string(id)+", using invalid item");
	}

	itemTraits itemInfo = itemManifest.at(id);

	this->name = std::get<0>(itemInfo);
	this->description = std::get<1>(itemInfo);
	this->weight = std::get<2>(itemInfo);
	this->size = std::get<3>(itemInfo);
	this->types = std::get<4>(itemInfo);
	this->primaryType = this->getPrimaryType();
	this->createAttributeSet(this->getAttributes(),std::get<5>(itemInfo));

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

std::string Item::getTypeAsString() {
	return itemTypeMap.at(this->primaryType);
}

std::string Item::getInfo() {
	std::string infoString = 
	" Name:\t"+this->name+"\n" +
	" Type:\t"+this->getTypeAsString()+"\n" +
	" Weight:\t"+std::to_string(this->weight)+"\n" +
	" Size:\t"+std::to_string(this->size);
	return infoString;
}

std::string Item::getName() {
	//For advanced item types with names that may be different than default
	return this->name;
}
std::string Item::getPlural() {
	if(this->name[this->name.size()-1] == 's') return this->name+"'";
	if(issymbol(this->name[this->name.size()-1])) return this->name;
	return this->name+'s';
}
std::string Item::getPluralDisplayName() {
	return replace(this->getPlural(),' ',"");
}
std::string Item::getDisplayName() {
	return replace(this->name,' ',"");
}
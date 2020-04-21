#include "Item.h"
#include "ItemManifest.h"
#include "../common/Common.h"
#include "StatusEffect.h"
#include "../tools/Utility.h"

#include <map>
#include <tuple>
#include <ctime>

std::map<ItemType,std::string> ItemTypeMap = {
	{I_FOOD,"Food"},
	{I_FRUIT,"Fruit"},
	{I_POTION,"Potion"},
	{I_CONSUMABLE,"Consumable"},
	{I_AMMO,"Ammunition"},
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
	{I_PLANTABLE,"Plantable"},
};

void checkItemTypes() {
	for(flag i=0;i<log2((int)I_END);i++) {
		if(ItemTypeMap.find((ItemType)pow(2,i)) == ItemTypeMap.end()) {
			debug("WARNING Item type string not found for type: "+std::to_string(i));
		}
	}
}

Item::Item(int _id): id(_id) {

	if(id >= itemManifest.size()) {
		debug("ERROR: Adding invalid item with id "+std::to_string(id)+", using invalid item");
		id = 0;
	}

	itemTraits itemInfo = itemManifest.at(id);

	this->name = std::get<0>(itemInfo);
	this->description = std::get<1>(itemInfo);
	this->weight = std::get<2>(itemInfo);
	this->size = std::get<3>(itemInfo);
	this->types = std::get<4>(itemInfo);
	// this->primaryType = this->getPrimaryType();
	// this->createAttributeSet(std::get<5>(itemInfo));
	this->attributes = std::get<5>(itemInfo);
	this->effects = std::get<6>(itemInfo);

	/*std::vector<ItemAttribute> attribEnums = attributeLookup.at(this->primaryType);
	std::vector<int> attribValues = std::get<4>(itemInfo);

	for(int i=0;i<attribEnums.size();i++) {
		this->attributeValues.insert({attribEnums.at(i),attribValues.at(i)});
	}
*/

	this->created = TBAGame->logicTicks;

}

std::vector<StatusEffect*> Item::getEffectsOnAction(Action action) {
	std::vector<StatusEffect*> effectsOnAction;
	if(this->effects.find(action) != this->effects.end()) {
		for(int i=0;i<this->effects.at(action).size();i++) {
			std::vector<float> effectValues = this->effects.at(action).at(i);
			EffectType type = (EffectType)effectValues.at(0);
			float magnitude = effectValues.at(1);
			float duration = TBAGame->convert(1000*effectValues.at(2));
			float period = 0;
			if(effectValues.size() == 4) period = TBAGame->convert(1000*effectValues.at(3));
			effectsOnAction.push_back(new StatusEffect(type,magnitude,duration,period));
		}
	}
	return effectsOnAction;
}

bool Item::hasEffectOnAction(Action action) {
	return this->effects.find(action) != this->effects.end();
	// return type | this->getAttribute(ATTRIB_CONSUME_EFFECT);
}

void Item::createAttributeSet(const std::vector<ItemAttribute> &attribs,const std::vector<float> &attribValues) {

	// If list of attribute values received from item manifest
	// is longer than list of attributes from attribute lookup map,
	// trailing attributes and values will be ignored

	int attribCount = std::min(attribs.size(),attribValues.size());

	for(int i=0;i<attribCount;i++) {
		this->attributes.insert({attribs.at(i),attribValues.at(i)});
	}
}

bool Item::hasAttribute(ItemAttribute attrib) {
	//return (this->attributeValues.find(attrib) != this->attributeValues.end());
	return (this->attributes.find(attrib) != this->attributes.end());
}

float Item::getAttribute(ItemAttribute attrib) {

	if(this->hasAttribute(attrib)) {
		return this->attributes.at(attrib);
	}
	return -1;

}

std::vector<std::string> Item::getTypes() {
	std::vector<std::string> typevec;
	for(int i=0;i<log2((int)I_END);i++) {
		if(this->hasType((ItemType)pow(2,i))) typevec.push_back(ItemTypeMap.at((ItemType)pow(2,i)));
	}
	return typevec;
}

std::string Item::getInfo() {
	std::string infoString = 
	" Name:\t"+this->name+"\n" +
	" Type:\t"+join(", ",this->getTypes())+"\n" +
	" Weight:\t"+std::to_string(this->weight)+"\n" +
	" Size:\t"+std::to_string(this->size);
	return infoString;
}

std::string Item::getName() {
	//For advanced item types with names that may be different than default
	return this->name;
}
std::string Item::getFormattedName() {
	//For advanced item types with names that may be different than default
	return TBAGame->colorKey+("w"+this->name)+TBAGame->colorKey;//TBAGame->colorKey;
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
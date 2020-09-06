#include "Item.h"
#include "ItemManifest.h"
#include "../common/Common.h"
#include "StatusEffect.h"
#include "../tools/Utility.h"

#include <map>
#include <tuple>
#include <ctime>
#include <fstream>

std::map<ItemType,std::string> ItemTypeMap = {
	{I_FOOD,"Food"},
	{I_FRUIT,"Fruit"},
	{I_POTION,"Potion"},
	{I_CONSUMABLE,"Consumable"},
	{I_AMMO,"Ammunition"},
	{I_VEGETABLE,"Vegetable"},
	{I_WEAPON,"Weapon"},
	{I_EQUIPMENT,"Equipment"},
	{I_WEAPON_MELEE,"Melee"},
	{I_WEAPON_RANGED,"Ranged"},
	{I_WEAPON_SWORD,"Sword"},
	{I_WEAPON_BOW,"Bow"},
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

bool importItems() {
	const std::string itemFile = "assets/objects/items.ead";
	std::string line;
	std::ifstream itemRaw(itemFile);
	if(itemRaw.is_open()) {
		
		bool reading = false;
		
		const std::string name_default = "ITEM_NAME_UNDEFINED";
		const std::string desc_default = "ITEM_DESC_UNDEFINED";
		int weight_default = 0;
		int size_default = 0;
		flag types_default = I_END;
		
		//Default attribs and effects
		std::map<ItemAttribute,float> attributes_default = {};
		std::map<Action,std::vector<std::vector<float>>> effects_default = {};
		
		std::string name,desc;
		int weight,size;
		flag types;
		std::map<ItemAttribute,float> attributes = {};
		std::map<Action,std::vector<std::vector<float>>> effects = {};
			
		std::vector<std::string> lines;
		std::vector<std::string> tline;
	
		while(std::getline(itemRaw,line)) {
			
			lines.push_back(line);
			
		}
		for(int i=0;i<lines.size();i++) {
			
			name = name_default;
			desc = desc_default;
			weight = weight_default;
			types = types_default;
			attributes = attributes_default;
			effects = effects_default;
			
			line = lines.at(i);
			

			if(line.size() < 2) continue;
			
			//ignore comments
			if(line.substr(0,2) == "//") continue;
			
			//ignore non-items
			if(line.substr(1,4) == "ITEM") {
				reading = true;
				//Ignore brace closure and newline char
				name = line.substr(6,line.size()-1);
				name.erase(name.size()-1);
				debug("Reading "+name);
				continue;
			}
			
			tline = split(':',line);
			if(tline.at(0) == "DESC") {
				name = tline.at(1);
			} else if(tline.at(0) == "WEIGHT") {
				weight = std::atoi(tline.at(1));
			}else if(tline.at(0) == "SIZE") {
				size = std::atoi(tline.at(1));
			}else if(tline.at(0) == "TYPES") {
				//Reverse lookup from attribute map
			}
			
			
			
		}
	} else {
		debug("Failed to open item file: "+itemFile);
		return false;
	}
	return true;
}

Item::Item(int _id): id(_id) {

	if(id >= itemManifest.size()) {
		debug("ERROR: Adding invalid item with id "+std::to_string(id)+", using invalid item");
		id = 0;
	}
	
	this->UUID = TBAGame->itemTotal++;

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

ItemType Item::getWeaponType() {
	std::vector<ItemType> weaponTypes = {I_WEAPON_SWORD,I_WEAPON_BOW};
	for(int i=0;i<weaponTypes.size();i++) {
		if(this->hasType(weaponTypes.at(i))) return weaponTypes.at(i);
	}
	return I_END;
}

std::string Item::getInfo() {
	std::string infoString = 
	" Name:\t"+this->getFormattedName()+"\n" +
	" Type:\t"+join("\n\t --\t",this->getTypes())+"\n" +
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

using namespace ItemUtils;

bool ItemUtils::hasType(int id,ItemType type) {
	if(id >= itemManifest.size()) {
		debug("ERROR (ItemUtils::hasType()): Checking type of invalid item");
		return false;
	}

	itemTraits itemInfo = itemManifest.at(id);
	flag types = std::get<4>(itemInfo);

	return (types & type) == type; 
}
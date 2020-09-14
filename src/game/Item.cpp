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
	{I_MINERAL,"Mineral"},
	{I_AMMO,"Ammo"},
	{I_VEGETABLE,"Vegetable"},
	{I_WEAPON,"Weapon"},
	{I_EQUIPMENT,"Equipment"},
	{I_WEAPON_MELEE,"Melee"},
	{I_WEAPON_RANGED,"Ranged"},
	{I_WEAPON_SWORD,"Sword"},
	{I_WEAPON_BOW,"Bow"},
	{I_ARMOR,"Armor"},	
	{I_ARMOR_HEAD,"Head armor"},	
	{I_ARMOR_BODY,"Body armor"},	
	{I_ARMOR_LEGS,"Leg armor"},	
	{I_ARMOR_FEET,"Foot armor"},
	{I_CRAFTING,"Crafting"},
	{I_METAL,"Metal"},
	{I_INGREDIENT,"Ingredient"},
	{I_CURRENCY,"Currency"},
	{I_CONTAINER,"Container"},
	{I_PLANTABLE,"Plantable"},
};

std::map<ItemAttribute,std::string> ItemAttributeMap = {
	{ATTRIB_DAMAGE,"Damage"},
	{ATTRIB_RANGE,"Range"},
	{ATTRIB_DEFENSE,"Defense"},
	{ATTRIB_CROP_GROWTH_TIME,"Growth time"},
	{ATTRIB_CROP_YIELD_ID,"Yield"},
	{ATTRIB_CROP_YIELD_AMOUNT,"Yield amount"},
};

std::map<Action,std::string> ItemActionMap = {
	{ACTION_CONSUME,"Consume"},
};

std::map<EffectType,std::string> ItemEffectMap = {
	{EFFECT_HEALING,"Healing"},
	{EFFECT_DAMAGE,"Harming"},
	{EFFECT_BURN,"Burning"},
};

void checkItemTypes() {
	debug("Checking item types...");
	//Verifies that all item traits have string mappings
	//Logs all enum values and string mappings for each mapped trait
	std::string typeLogFile = "assets/objects/item_traits.txt";
	clear_file(typeLogFile);
	std::ofstream types(typeLogFile);
	types << "Types:" << std::endl;
	flag i;
	int good = 0;
	for(i=1;i<I_END;i = i<<1) {
		if(ItemTypeMap.find((ItemType)i) == ItemTypeMap.end()) {
			TBA_throw(WARN_DEFAULT,__PRETTY_FUNCTION__,"Item type string not found for type "+std::to_string((int)log2(i)));
			types << log2(i) << ": " << "Type string not found" << std::endl;
		} else {
			good++;
			types << log2(i) << ": " << ItemTypeMap.at((ItemType)i) << std::endl;
		}
	}
	debug("Found "+std::to_string(good)+" item type(s)");
	good = 0;
	types << std::endl << "Attributes: " << std::endl;
	for(i=0;i<ATTRIB_END;i++) {
		if(ItemAttributeMap.find((ItemAttribute)i) == ItemAttributeMap.end()) {
			TBA_throw(WARN_DEFAULT,__PRETTY_FUNCTION__,"Item attribute string not found for attribute "+std::to_string(i));
			types << i << ": " << "Attribute string not found" << std::endl;
		} else {
			good++;
			types << i << ": " << ItemAttributeMap.at((ItemAttribute)i) << std::endl;
		}
	}
	debug("Found "+std::to_string(good)+" item attribute(s)");
	good = 0;
	types << std::endl << "Actions: " << std::endl;
	for(i=0;i<ACTION_END;i++) {
		if(ItemActionMap.find((Action)i) == ItemActionMap.end()) {
			TBA_throw(WARN_DEFAULT,__PRETTY_FUNCTION__,"Item action string not found for action "+std::to_string(i));
			types << i << ": " << "Action string not found" << std::endl;
		} else {
			good++;
			types << i << ": " << ItemActionMap.at((Action)i) << std::endl;
		}
	}
	debug("Found "+std::to_string(good)+" action(s)");
	good = 0;
	types << std::endl << "Effects: " << std::endl;
	for(i=0;i<EFFECT_END;i++) {
		if(ItemEffectMap.find((EffectType)i) == ItemEffectMap.end()) {
			TBA_throw(WARN_DEFAULT,__PRETTY_FUNCTION__,"Item effect string not found for effect "+std::to_string(i));
			types << i << ": " << "Effect string not found" << std::endl;
		} else {
			good++;
			types << i << ": " << ItemEffectMap.at((EffectType)i) << std::endl;
		}
	}
	debug("Found "+std::to_string(good)+" status effect(s)");
	types.close();
	debug("Done checking item types");
}

bool importItems() {
	int itemCount = 0;
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
						
			line = lines.at(i);
			

			if(line.size() < 2) {
				if(reading) {
					//Empty lines while reading means items is done
					// debug("Creating item "+name);
					itemManifest.push_back({name,desc,weight,size,types,attributes,effects});
					//Begin new item
					name = name_default;
					desc = desc_default;
					weight = weight_default;
					types = types_default;
					attributes = attributes_default;
					effects = effects_default;
				}
				reading = false;
				continue;
			}
			
			//Ignore whitespace
			line = replace(line,'\t',"");
			line = replace(line,'\n',"");
			
			//ignore comments
			if(line.substr(0,2) == "//") continue;
			
			//ignore non-items
			if(line.substr(0,5) == "[ITEM") {
				if(reading) {
					// debug("Creating item "+name);
					itemManifest.push_back({name,desc,weight,size,types,attributes,effects});
					//Begin new item
					name = name_default;
					desc = desc_default;
					weight = weight_default;
					types = types_default;
					attributes = attributes_default;
					effects = effects_default;
				}
				reading = true;
				//Ignore brace closure and newline char
				name = line.substr(6,line.size()-1);
				name.erase(name.size()-1);
				// debug("Reading "+name);
				continue;
			}
			
			if(!reading and line[0] == '[') {
				// debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Ignoring malformed definition \""+line+"\"");
				TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Ignoring malformed definition \""+line+"\"");
				continue;
			}
			
			if(!reading) continue;
			
			tline = split(':',line);
			std::string trait = tline.at(0);
			if(trait == "DESC") {
				if(tline.size() > 1) {
					desc = tline.at(1);
				} else {
					// debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Unexpected EOL, missing description \""+name+"\"");
					TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Unexpected EOL, missing description \""+name+"\"");
				}
			} else if(trait == "WEIGHT") {
				if(tline.size() > 1) {
						if(!TBA_stoi(tline.at(1),weight)) {
							// debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Non-integral weight value \""+tline.at(1)+"\" for item \""+name+"\"");
							TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Non-integral weight value \""+tline.at(1)+"\" for item \""+name+"\"");
							continue;
						}
					} else {
					// debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Unexpected EOL, missing weight value \""+name+"\"");
					TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Unexpected EOL, missing weight value \""+name+"\"");
				}
			}else if(trait == "SIZE") {
				if(tline.size() > 1) {
					if(!TBA_stoi(tline.at(1),size)) {
							// debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Non-integral size value \""+tline.at(1)+"\" for item \""+name+"\"");
							TBA_throw(ERR_DEFAULT,itemFile,"Non-integral size value \""+tline.at(1)+"\" for item \""+name+"\"");
							continue;
						}
				} else {
					// debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Unexpected EOL, missing size value \""+name+"\"");
					TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Unexpected EOL, missing size value \""+name+"\"");
				}
			}else if(trait == "TYPES") {
				bool found_type;
				//Reverse lookup from type map
				for(int j=1;j<tline.size();j++) {
					//If we determined a type for this string
					found_type = false;
					for(flag k=1;k < I_END;k = k<<1) {
						//Skip item types with no string lookup
						if(ItemTypeMap.find((ItemType)k) == ItemTypeMap.end()) continue;
						if(tline.at(j) == toLower(ItemTypeMap.at((ItemType)k))) {
							types = types | k;
							found_type = true;
							break;
						}
					}
					// if(!found_type) debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Unknown type token \""+tline.at(j)+"\"");
					if(!found_type) TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Unknown type token \""+tline.at(j)+"\"");
				}
			}else if(trait == "ATTRIBS") {
				if(tline.size() <= 1) {
					// debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Unexpected EOL, missing attribute definition for item \""+name+"\"");
					TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Unexpected EOL, missing attribute definition for item \""+name+"\"");
					continue;
				}
				bool found_attrib;
				//Reverse lookup from type map
				for(int j=1;j<tline.size();j++) {
					//Skip attribute values
					if(isdigit(tline.at(j))) continue;
					
					//If we don't have any attribute strings that match this attribute
					found_attrib = false;
					
					for(int k=0;k < ATTRIB_END;k++) {
						if(ItemAttributeMap.find((ItemAttribute)k) == ItemAttributeMap.end()) continue;
						if(tline.at(j) == toLower(ItemAttributeMap.at((ItemAttribute)k))) {
							//UNSAFE INDEXING!!!!!!!!!!!
							found_attrib = true;
							if(j+1 < tline.size()) {
								attributes.insert({(ItemAttribute)k,std::stof(tline.at(j+1))});
								break;
							} else {
								// debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Unexpected EOL, missing attribute value for attribute \""+tline.at(j)+"\"");
								TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Unexpected EOL, missing attribute value for attribute \""+tline.at(j)+"\"");
							}
						}
					}
					// if(!found_attrib) debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Unknown attribute token \""+tline.at(j)+"\"");
					if(!found_attrib) TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Unknown attribute token \""+tline.at(j)+"\"");
				}
			}else if(trait == "EFFECTS") {

				if(tline.size() <= 1) {
					// debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Unexpected EOL, missing effect definition for item \""+name+"\"");
					TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Unexpected EOL, missing effect definition for item \""+name+"\"");
					continue;
				}
				
				//In order to apply multiple effects from one action
				//We need to add another loop that catches ALL effects not just the one immediately after an action 

				bool found_action = false;
				bool found_effect = false;
				Action action = ACTION_END;
				EffectType effect = EFFECT_END;
				std::vector<std::vector<float>> new_effects = {};
				//Reverse lookup from type map
				for(int j=1;j<tline.size();j++) {
					for(int k=0;k<ACTION_END;k++) {
						if(ItemActionMap.find((Action)k) == ItemActionMap.end()) continue;
						if(tline.at(j) == toLower(ItemActionMap.at((Action)k))) {
							if(found_action) {
								if(action != (Action)k) {
									//If we were reading an action when we found a new action, insert our current effect and start a new one
									if(effects.find(action) != effects.end()) {
										for(int n=0;n<effects.at(action).size();n++) {
											new_effects.push_back(effects.at(action).at(n));
										}
										effects.erase(effects.find(action));
									}
									effects.insert({action,new_effects});
									new_effects = {};
								}
							}
							//This is an action
							found_action = true;
							action = (Action)k;
							break;
						}
					}
					for(int k=0;k<EFFECT_END;k++) {
						if(ItemEffectMap.find((EffectType)k) == ItemEffectMap.end()) continue;
						if(tline.at(j) == toLower(ItemEffectMap.at((EffectType)k))) {
							if(!found_action) {
								// debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Unexpected effect, expected action for effect \""+tline.at(j)+"\", ignoring");
								TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Unexpected effect, expected action for effect \""+tline.at(j)+"\", ignoring");
								break;
							}
							//This is an Effect
							found_effect = true;
							effect = (EffectType)k;
							if(j+2 < tline.size()) {
								float magnitude = std::stof(tline.at(j+1));
								float duration = std::stof(tline.at(j+2));
								float period = 0;
								try {
									period = std::stof(tline.at(j+3));
								} catch(const std::exception &e) {
									
								}
								new_effects.push_back({(float)effect,magnitude,duration,period});
							} else {
								// debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Unexpected EOL, missing effect values for effect \""+tline.at(j)+"\"");
								TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Unexpected EOL, missing effect values for effect \""+tline.at(j)+"\"");
							}
						}
					}
					if(!found_action and !found_effect) {
						// debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Unknown attribute or effect token \""+tline.at(j)+"\"");
						TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Unknown attribute or effect token \""+tline.at(j)+"\"");
						break;
					}
				}
				//End of line reached
				if(effects.find(action) != effects.end()) {
					for(int n=0;n<effects.at(action).size();n++) {
						new_effects.push_back(effects.at(action).at(n));
					}
					effects.erase(effects.find(action));
				}
				if(found_action) {
					effects.insert({action,new_effects});
				} else {
					// debug("Error ("+itemFile+", line "+std::to_string(i+1)+"): Unexpected EOL, expected action token for \""+name+"\"");
					TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Unexpected EOL, expected action token for \""+name+"\"");
				}
			} else {
				TBA_throw(ERR_DEFAULT,itemFile+", line "+std::to_string(i+1),"Unknown trait \""+trait+"\"");
			}
			
			
		}
	} else {
		debug("Failed to open item file: "+itemFile);
		return false;
	}
	debug("Found "+std::to_string(itemManifest.size())+" items");
	debug("Done importing items");
	return true;
}

Item::Item(int _id): id(_id) {

	if(id >= itemManifest.size()) {
		// debug("ERROR: Adding invalid item with id "+std::to_string(id)+", using invalid item");
		TBA_throw(ERR_DEFAULT,__PRETTY_FUNCTION__,"Adding invalid item with id "+std::to_string(id)+", using invalid item");
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
			float magnitude;
			float duration;
			float period;
			
			if(effectValues.size() == 4) {
				period = TBAGame->convert(1000*effectValues.at(3));
				duration = TBAGame->convert(1000*effectValues.at(2)*effectValues.at(3));
				if(effectValues.at(2) == 0) {
					// debug("ERROR (Item::getEffectsOnAction()): Divide by zero");
					TBA_throw(ERR_DIV_BY_ZERO,__PRETTY_FUNCTION__);
					return {};
				}
				magnitude = effectValues.at(1)/effectValues.at(2);
			} else {
				magnitude = effectValues.at(1);
				period = 0;
				duration = TBAGame->convert(1000*effectValues.at(2));
			}

		
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
		// debug("ERROR (ItemUtils::hasType()): Checking type of invalid item");
		TBA_throw(ERR_DEFAULT,__PRETTY_FUNCTION__,"Checking type of invalid item");
		return false;
	}

	itemTraits itemInfo = itemManifest.at(id);
	flag types = std::get<4>(itemInfo);

	return (types & type) == type; 
}
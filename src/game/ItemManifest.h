#pragma once

#include <vector>
#include <string>
#include <map>
#include <tuple>

typedef unsigned long int flag;

//==========
//ATTRIBUTES
//==========

// Attributes are values that represent specific qualities about that item
//
// For example, a weapon will have a DAMAGE attribute
// whose value will be found at the ATTRIB_DAMAGE key in the item's attribute map

enum Action {
	ACTION_CONSUME,
	ACTION_END,
};

enum ItemAttribute {
	ATTRIB_DAMAGE,
	ATTRIB_RANGE,
	ATTRIB_DEFENSE,
	ATTRIB_CROP_GROWTH_TIME,
	ATTRIB_CROP_YIELD_ID,
	ATTRIB_CROP_YIELD_AMOUNT,
	ATTRIB_END,
};

//==========
// 	TYPES
//==========


//Supports up to 64 different item types
enum ItemType: flag {

	//Should go from General type to Specific type in descending order
	// Item picks up attributes of lowest type

	I_FOOD =			1 << 0,
	I_FRUIT =			1 << 1,
	I_VEGETABLE =	1 << 2,
	I_POTION	=		1 << 21,
	I_CONSUMABLE =		1 << 22,
	
	I_WEAPON = 				1 << 3,
	I_EQUIPMENT =			1 << 4,
	I_WEAPON_MELEE =		1 << 5,
	I_WEAPON_SWORD =	1 << 6,
	
	I_WEAPON_RANGED =	1 << 7,
	I_WEAPON_BOW =		1 << 8,
	
	I_ARMOR =					1 << 9,
	I_ARMOR_HEAD =			1 << 10,
	I_ARMOR_BODY =			1 << 11,
	I_ARMOR_LEGS =			1 << 12,
	I_ARMOR_FEET =			1 << 13,
	
	I_CRAFTING =		1 << 14,
	I_METAL =			1 << 15,
	I_INGREDIENT = 	1 << 16,
	I_CURRENCY = 	1 << 17,
	I_PLANTABLE = 	1 << 18,
	I_AMMO = 			1 << 19,
	
	I_CONTAINER =	1 << 20,
	I_END =				1 << 23,

};

inline ItemType operator|(ItemType f1,ItemType f2) {
	return (ItemType)((flag)f1 | (flag)f2);
}
inline ItemType operator&(ItemType f1,ItemType f2) {
	return (ItemType)((flag)f1 & (flag)f2);
}
inline bool operator==(ItemType f1,ItemType f2) {
	return (f1 ^ f2) == 0;
}
inline bool operator!=(ItemType f1,ItemType f2) {
	return (f1 ^ f2) > 0;
}


extern std::map<ItemType,std::vector<ItemAttribute>> attributeLookup;

//Itemtraits for pulling items from item Manifest

typedef std::tuple<
		std::string,				//Name
		std::string,				//desc
		int,						//Weight
		int,						//Size
		flag,						//Type 
		// std::vector<float>				//Type-specific traits (attributes)
		std::map<ItemAttribute,float>,
		std::map<Action,std::vector<std::vector<float>>>
> itemTraits;

extern std::vector<itemTraits> itemManifest;
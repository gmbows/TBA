#pragma once

#include <vector>
#include <string>
#include <map>
#include <tuple>

typedef unsigned int flag;

enum itemType: flag {

	//Should go from General type to Specific type in descending order
	// Item picks up attributes of lowest type

	I_FOOD =		1 << 0,
	I_FRUIT =		1 << 1,
	I_VEGETABLE	=	1 << 2,
	
	I_WEAPON = 			1 << 3,
	I_EQUIPMENT =		1 << 4,
	I_WEAPON_MELEE =	1 << 5,
	I_WEAPON_SWORD =	1 << 6,
	I_WEAPON_RANGED =	1 << 7,
	I_WEAPON_BOW =		1 << 8,
	
	I_ARMOR =			1 << 9,
	I_ARMOR_HEAD =		1 << 10,
	I_ARMOR_BODY =		1 << 11,
	I_ARMOR_LEGS =		1 << 12,
	I_ARMOR_FEET =		1 << 13,
	
	I_CRAFTING =	1 << 14,
	I_METAL =		1 << 15,
	I_INGREDIENT = 	1 << 16,
	I_CURRENCY = 	1 << 17,
	
	I_CONTAINER =	1 << 18,
	I_END =			1 << 19,

};

inline itemType operator|(itemType f1,itemType f2) {
	return (itemType)((flag)f1 | (flag)f2);
}
inline itemType operator&(itemType f1,itemType f2) {
	return (itemType)((flag)f1 & (flag)f2);
}
inline bool operator==(itemType f1,itemType f2) {
	return (f1 ^ f2) == 0;
}
inline bool operator!=(itemType f1,itemType f2) {
	return (f1 ^ f2) > 0;
}

//==========
//ATTRIBUTES
//==========

// Attributes (determined by primary (first) item type) are values that represent indices in each item's
// sub type vector that hold specific qualities about that item
//
// For example, a weapon will have a DAMAGE attribute
// whose value will be found at index 0 in the item's attribute (sub type) vector
//
// Because of limited space in the attribute vector,
// attributes must share enumerations (indices) which would make it
// possible to find the damage attribute of a fruit, so a map is created
// to identify which attributes can be found in the attribute vectors of each
// primary item type.
//
// Fetching the attribute value of an item that doesn't have said attribute
// will return -1

//ATTRIBUTE INDEX MUST FOLLOW DIRECTLY AFTER INITIAL ATTRIBUTE ENUMERATION
enum itemAttribute {
	ATTRIB_DAMAGE,
	ATTRIB_RANGE,
	ATTRIB_DEFENSE,
};

extern std::map<itemType,std::vector<itemAttribute>> attributeLookup;

//Itemtraits for pulling items from item Manifest

typedef std::tuple<
		std::string,				//Name
		int,						//Weight
		int,						//Size
		flag,						//Type 
		std::vector<float>				//Type-specific traits (attributes)
> itemTraits;

extern std::vector<itemTraits> itemManifest;
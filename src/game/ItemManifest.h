#pragma once

#include <vector>
#include <string>
#include <map>
#include <tuple>

enum itemType {

	FOOD,
		FRUIT,
		VEGETABLE,
	EQUIPMENT,
		WEAPON,
			MELEE,
				SWORD,
			RANGED,
				BOW,
		ARMOR,
	CRAFTING,
		METAL,
	INGREDIENT,
	CONTAINER,

};

//==========
//ATTRIBUTES
//==========

// Attributes (determined by primary [first] item type) are values that represent indices in each item's
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
	DAMAGE,
	RANGE,
};

extern std::map<itemType,std::vector<itemAttribute>> attributeLookup;

//Itemtraits for pulling items from item Manifest

typedef std::tuple<
		std::string,						//Name
		int,								//Weight
		int,								//Size
		std::vector<itemType>,	//Type vector
		std::vector<float>				//Type-specific traits (attributes)
> itemTraits;

extern std::map<int,itemTraits> itemManifest;
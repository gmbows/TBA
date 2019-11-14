#include "ItemManifest.h"

std::map<itemType,std::vector<itemAttribute>> attributeLookup {

	{I_WEAPON,{ATTRIB_DAMAGE,ATTRIB_RANGE}},
	{I_ARMOR,{ATTRIB_DEFENSE}},
	{I_FOOD,{}},
	{I_CRAFTING,{}},
	{I_CONTAINER,{}},

};

//std::map<int,itemTraits> itemManifest = {
std::vector<itemTraits> itemManifest = {

		//name,weight,size, types...,{Attribute values if applicable}
	{"None",0,0,I_END,{}},
	{"Apple",1,4,I_FOOD | I_FRUIT | I_INGREDIENT,{}},
	{"Copper coin",1,4,I_FOOD | I_FRUIT | I_INGREDIENT,{}},
	{"Iron Ore",3,3,I_CRAFTING | I_METAL | I_INGREDIENT,{}},
	{"Iron Arrow",3,3,I_CRAFTING | I_METAL | I_INGREDIENT,{}},
	{"Rusted Longsword",8,3,I_WEAPON | I_WEAPON_MELEE | I_WEAPON_SWORD | I_EQUIPMENT,{5,1.5}},
	{"Iron Dagger",4,1,I_WEAPON | I_WEAPON_MELEE | I_WEAPON_SWORD | I_EQUIPMENT,{2,1.2}},
	{"Glass vial (empty)",3,3,I_CONTAINER,{}},
	{"Cracked Longbow",3,3,I_WEAPON | I_WEAPON_RANGED | I_WEAPON_BOW | I_EQUIPMENT,{2,-1}},
	{"Old Knight's Helmet",3,3,I_ARMOR | I_ARMOR_HEAD | I_EQUIPMENT,{5}},

};

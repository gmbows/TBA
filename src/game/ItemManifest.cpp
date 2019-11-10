#include "ItemManifest.h"

std::map<itemType,std::vector<itemAttribute>> attributeLookup {

	{I_WEAPON,{ATTRIB_DAMAGE,ATTRIB_RANGE}},
	{I_FOOD,{}},
	{I_CRAFTING,{}},
	{I_CONTAINER,{}},

};

//std::map<int,itemTraits> itemManifest = {
std::vector<itemTraits> itemManifest = {

		//name,weight,size, {Primary type, sub types...},{Attribute values}
	{"Apple",1,4,I_FOOD | I_FRUIT | I_INGREDIENT,{}},
	{"Iron Ore",3,3,I_CRAFTING | I_METAL | I_INGREDIENT,{}},
	{"Iron Arrow",3,3,I_CRAFTING | I_METAL | I_INGREDIENT,{}},
	{"Rusted Longsword",8,3,I_WEAPON | I_MELEE | I_SWORD | I_EQUIPMENT,{5,1.5}},
	{"Glass vial (empty)",3,3,I_CONTAINER,{}},
	{"Cracked Longbow",3,3,I_WEAPON | I_RANGED | I_BOW | I_EQUIPMENT,{2,-1}},

};

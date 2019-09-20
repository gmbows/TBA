#include "ItemManifest.h"

std::map<itemType,std::vector<itemAttribute>> attributeLookup {

	{WEAPON,{DAMAGE,RANGE}},
	{FOOD,{}},
	{CRAFTING,{}},
	{CONTAINER,{}},

};

std::map<int,itemTraits> itemManifest = {

		//name,weight,size, {Primary type, sub types...},{Attribute values}
	{0,{"Apple",1,4,{FOOD,FRUIT,INGREDIENT},{}}},
	{1,{"Iron Ore",3,3,{CRAFTING,METAL,INGREDIENT},{}}},
	{2,{"Rusted Longsword",8,3,{WEAPON,MELEE,SWORD,EQUIPMENT},{5,1.5}}},
	{3,{"Glass vial (empty)",3,3,{CONTAINER},{}}},
	{4,{"Cracked Longbow",3,3,{WEAPON,RANGED,BOW,EQUIPMENT},{2,-1}}},
	

};

#include "ItemManifest.h"
#include "StatusEffect.h"

std::map<itemType,std::vector<itemAttribute>> attributeLookup {

	{I_WEAPON,{	ATTRIB_DAMAGE,
							ATTRIB_RANGE}},
							
	{I_ARMOR,{ATTRIB_DEFENSE}},
	
	{I_FOOD,{}},
	{I_PLANTABLE,{	ATTRIB_CROP_GROWTH_TIME,
								ATTRIB_CROP_YIELD_ID,
								ATTRIB_CROP_YIELD_AMOUNT,}},
	{I_CRAFTING,{}},
	{I_CONTAINER,{}},

};


//std::map<int,itemTraits> itemManifest = {
std::vector<itemTraits> itemManifest = {

		//name,weight,size, types...,{Attribute values if applicable}
	{"ITEM_INVALID","ITEM_DESC_INVALID",0,0,I_END,{}},
	{"Apple","A shiny red apple.", 1,4,I_FOOD | I_FRUIT | I_INGREDIENT | I_PLANTABLE,{}},
	{"Healing Potion","A bright red, glowing, liquid in a small glass vial. It glistens in sunlight.", 1,4,I_FOOD | I_INGREDIENT ,{}},
	{"Wheat","A bundle of dry, golden, wheat", 1,4,I_FOOD | I_FRUIT | I_INGREDIENT,{}},
	{"Wheat Seed","A handful of small wheat seeds.", 1,4, I_INGREDIENT | I_PLANTABLE,{100,2,3}},
	{"Copper Coin","A scuffed, bent, copper coin. It a building printed on it.", 1,4,I_CURRENCY | I_METAL,{}},
	{"Iron Ore","A smooth looking grey rock. Quite heavy.", 3,3,I_CRAFTING | I_METAL | I_INGREDIENT,{}},
	{"Iron Arrow","A wood-shafted arrow with an iron tip. Could probably do some damage.",3,3,I_WEAPON_BOW | I_AMMO | I_METAL,{}},
	{"Rusted Longsword","A dull, rusted sword. It looks like it hasn't been touched for some time.",8,3,I_WEAPON | I_WEAPON_MELEE | I_WEAPON_SWORD | I_EQUIPMENT,{5,1.5}},
	{"Iron Dagger","A small iron dagger with a wooden grip.",4,1,I_WEAPON | I_WEAPON_MELEE | I_WEAPON_SWORD | I_EQUIPMENT,{2,1.2}},
	{"Glass vial (empty)","A tiny glass vial and a small cork. Could store a splash of liquid in here.",3,3,I_CONTAINER,{}},
	{"Cracked Longbow","A wooden bow. Probably won't survive more than a few shots.",3,3,I_WEAPON | I_WEAPON_RANGED | I_WEAPON_BOW | I_EQUIPMENT,{2,-1}},
	{"Old Knight's Helmet","An old iron helmet, probably used by a knight. Rusted around the hinges.",3,3,I_ARMOR | I_ARMOR_HEAD | I_EQUIPMENT,{5}},
	{"Old Knight's Chestplate","An iron chestplate, part of a full set of armor.",3,3,I_ARMOR | I_ARMOR_BODY | I_EQUIPMENT,{5}},
	{"Old Knight's Plated Leggings","Some dented iron leggings. Looks like they've taken a beating.",3,3,I_ARMOR | I_ARMOR_LEGS | I_EQUIPMENT,{5}},
	{"Old Knight's Boots","Some iron boots. They don't look very comfortable.",3,3,I_ARMOR | I_ARMOR_FEET | I_EQUIPMENT,{5}},

};

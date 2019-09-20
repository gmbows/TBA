#pragma once

#include "Item.h"
#include "ItemManifest.h"

struct Equipment {

	Item* head = nullptr;
	Item* body = nullptr;
	Item* legs = nullptr;
	Item* feet = nullptr;
	
	Item* primary = nullptr;
	Item* secondary = nullptr;

};
#pragma once

#include "ItemManifest.h"

struct AttributeSet {
	
	// Maps itemAttributes to values received from item manifest
	std::map<itemAttribute,float> attributeValues;
	
	AttributeSet(const std::vector<itemAttribute> &attribEnums, const std::vector<float> &attribValues);

	bool hasAttribute(itemAttribute);
	float getValue(itemAttribute);

};
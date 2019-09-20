#include "Attributes.h"

#include "../tools/StringFuncs.h"

AttributeSet::AttributeSet(const std::vector<itemAttribute> &attribEnums, const std::vector<float> &attribValues) {

	// If list of attribute values received from item manifest
	// is longer than list of attributes from attribute lookup map,
	// trailing attribute values will be ignored

	int attribCount = std::min(attribEnums.size(),attribValues.size());

	for(int i=0;i<attribCount;i++) {
		this->attributeValues.insert({attribEnums.at(i),attribValues.at(i)});
	}
}

bool AttributeSet::hasAttribute(itemAttribute attrib) {
	return (this->attributeValues.find(attrib) != this->attributeValues.end());
}

float AttributeSet::getValue(itemAttribute attrib) {
	if(this->hasAttribute(attrib)) {
		return this->attributeValues.at(attrib);
	}
	return -1;
}
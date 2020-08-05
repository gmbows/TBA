#include "ResourceNode.h"

std::string ResourceNode::getInfo() {
		return " Name:\t"+this->name + "\n" +
				"\tLocation:\t"+std::to_string((int)std::round(this->x)) + "," + std::to_string((int)std::round(this->y)) + "\n" +
				"\tResource:\t"+Item(this->primaryResource).getFormattedName()+"\n"+
				"\tProgress:\t"+std::to_string((int)(100*(this->progress/this->duration)))+"%\n"+
				"\tContents:"+this->inventory->toString();
	}
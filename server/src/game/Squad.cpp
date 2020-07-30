#include "Squad.h"
#include "GameObject.h"
#include "../tools/Utility.h"
#include "../common/Common.h"

GameObject* Squad::getMember(const std::string &_name) {
	std::string name = _name;
	if(autocomplete(name,TBAGame->convert(this->members))) {
		for(int i=0;i<this->members.size();i++) {
			if(this->members.at(i)->getName() == name) {
				return this->members.at(i);
			}
		}
	}
	return nullptr;
}

std::string Squad::serialize() {
	std::string name,membercount,smembers;
	std::string member;
	
	name = this->name;
	pad(name,' ',PAD_STR);
	
	membercount = std::to_string(this->members.size());
	pad(membercount,'0',PAD_INT);
	
	for(int i=0;i<this->members.size();i++) {
		if(this->members.at(i) == nullptr) continue;
		member = std::to_string(this->members.at(i)->objectID);
		// debug(i);
		pad(member,'0',PAD_INT);
		// debug(i);
		smembers += member;
	}
	
	return name+membercount+smembers;
}

void Squad::alert(GameObject *c) {
	Character *thisMember;
	//Ignore friendly fire (for now)
	if(this->isMember(c->getAsCharacter())) return;
	for(int i=0;i<this->members.size();i++) {
		thisMember = this->members.at(i);
		if(thisMember->getCharTarget() == nullptr or !thisMember->targetInRange() or !thisMember->getCharTarget()->isAlive() or (char*)thisMember->getCharTarget()->getCharTarget() != (char*)thisMember) {
			thisMember->setTarget(static_cast<Character*>(c));
			thisMember->addStatus(STATUS_COMBAT);
		}
	}
}
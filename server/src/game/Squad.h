#pragma once
#include "Character.h"

class Character;
class GameObject;

struct Squad {

	int id;
	std::string name;
	std::vector<Character*> members;
	
	std::string serialize();
	
	GameObject* getMember(const std::string&);
	std::vector<std::string> getMemberNames() {
		std::vector<std::string> mem;
		for(int i=0;i<this->members.size();i++) {
			mem.push_back(this->members.at(i)->getFormattedName());
		}
		return mem;
	}

	void alert(GameObject* c);
	
	std::string getInfo() {
		return this->name;
	}

	bool isMember(Character *c) {
		for(int i=0;i<this->members.size();i++) {
			if((char*)this->members.at(i) == (char*)c) return true;
		}
		return false;
	}
	
	void add(Character* c) {
		this->members.push_back(c);
		c->squad = this;
	}
	
	Squad(std::vector<Character*> m,std::string name = "NAME_UNASSIGNED"): members(m) {
		// this->id = 
		if(name == "NAME_UNASSIGNED") {
			if(this->members.size() > 0) this->name = this->members.at(0)->getName()+" Squad";
		}
	}

};
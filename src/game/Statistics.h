#pragma once

#include <tuple>

enum statType {
	STAT_STRENGTH,
	STAT_DEXTERITY,
};

struct Stat {

	statType type;
	int xp;
	int level;

	Stat(statType _type) {
		this->type = _type;
		this->xp = 0;
		this->level = 0;
	}

};

struct StatSet {

	Stat* strength;
	Stat* dexterity;

	StatSet() {
		this->strength = new Stat(STAT_STRENGTH);
		this->dexterity = new Stat(STAT_DEXTERITY);
	}

};
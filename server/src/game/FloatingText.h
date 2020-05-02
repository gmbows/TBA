#pragma once

#include "../common/Common.h"
#include "GameObject.h"
#include "../tools/Utility.h"

#include <random>
#include <ctime>
#include <tuple>

// RENDERED CLIENTSIDE

struct FloatingText: public GameObject {

	unsigned int created;
	const std::string message;
	int duration;
	float completion = 0;

	float defaultX,defaultY;

	int heightReached = 15;
	int dialogOffset = 0;
	int offsetY = 0;
	int offsetX = 0;
	int offsetXMult = 0;
	int offsetXsign = 1;
	SDL_Colour color;

	std::tuple<float,float> location;

	Character* trackSubject;
	bool tracking = false;

	//Damage numbers
	FloatingText(int _duration,const std::string& _message,std::tuple<float,float> _location,SDL_Colour _color);
	
	//Dialog
	FloatingText(int _duration,const std::string& _message,std::tuple<float,float> _location, Character* tracker);

	bool hasExpired() {
		return TBAGame->logicTicks >= this->created + (TBAGame->convert(this->duration*1000));
	}

	// void drawContent();

	// void update();

};

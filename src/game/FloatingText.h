#pragma once

#include "../common/Common.h"
#include "../ui/Screen.h"
#include "GameObject.h"
#include "../tools/StringFuncs.h"

#include <random>
#include <ctime>
#include <tuple>

struct FloatingText: public GameObject {

	Uint32 created;
	const std::vector<std::string> message;
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
	FloatingText(int _duration,const std::vector<std::string>& _message,std::tuple<float,float> _location,SDL_Colour _color = {200,200,200,255}):
		duration(_duration), message(_message), location(_location), color(_color), GameObject(UI_FLOATINGTEXT) {
		this->dialogOffset = 5;
		this->offsetXMult = rand()%16;
		this->offsetXsign = ((rand()%2) == 0)? -1 : 1;
		this->created = SDL_GetTicks();
		decompose(this->location,this->defaultX,this->defaultY);

	}
	//Dialog
	FloatingText(int _duration,const std::vector<std::string>& _message,std::tuple<float,float> _location, Character* tracker):
		duration(_duration), message(_message), location(_location), trackSubject(tracker), GameObject(UI_FLOATINGTEXT) {
		this->dialogOffset = 24;
		decompose(this->location,this->defaultX,this->defaultY);
		this->tracking = true;
		this->created = SDL_GetTicks();
	}

	bool hasExpired() {
		return SDL_GetTicks() >= this->created + (this->duration*1000);
	}

	void drawContent();

	void update();

};

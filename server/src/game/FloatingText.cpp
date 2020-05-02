#include "FloatingText.h"
#include "../tools/Utility.h"

//Damage
FloatingText::FloatingText(int _duration,const std::string& _message,std::tuple<float,float> _location,SDL_Colour _color):
	duration(_duration), message(_message), location(_location), color(_color), GameObject(UI_FLOATINGTEXT) {
	this->dialogOffset = 5;
	this->offsetXMult = rand()%16;
	this->offsetXsign = ((rand()%2) == 0)? -1 : 1;
	this->created = TBAGame->logicTicks;
	decompose(this->location,this->defaultX,this->defaultY);
}
//Dialog
FloatingText::FloatingText(int _duration,const std::string& _message,std::tuple<float,float> _location, Character* tracker):
	duration(_duration), message(_message), location(_location), trackSubject(tracker), GameObject(UI_FLOATINGTEXT) {
	this->dialogOffset = 24;
	decompose(this->location,this->defaultX,this->defaultY);
	this->tracking = true;
	this->created = TBAGame->logicTicks;
	this->color = {200,200,200,255};
}
#pragma once

#include <SDL2/SDL.h>

struct Clock {

	unsigned int ticks;
	unsigned int inline getTicks() {return ticks;}

	void setTime(unsigned int time) {
		this->ticks = time;
	}

	void advance(unsigned int time) {
		this->ticks += time;
	}

	Clock() {
		this->ticks = 0;
	}
};
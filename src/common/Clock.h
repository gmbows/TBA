#pragma once

#include <SDL2/SDL2.h>

struct Clock {

	uint32_t ticks;
	uint32_t inline getTicks() {return ticks;}

	void setTime(uint32_t time) {
		this->ticks = time;
	}

	Clock() {
		this->ticks = SDL_GetTicks();
	}
}
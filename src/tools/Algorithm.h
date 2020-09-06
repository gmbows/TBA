#pragma once
#include "Utility.h"

#include <SDL2/SDL.h>

#include <vector>
#include <queue>
#include <pthread.h>

class Character; 
class Tile; 

//====================
//		PATHFINDING
//====================

std::vector<Tile*> generatePath(Character*,float tx, float ty, bool adjacent);
bool characterCanReach(Character*,float,float,bool);

extern unsigned int node_count;
extern unsigned int trials;
extern unsigned int total_iter;

//heuristic function
float h(Tile *tile,Tile *start, Tile *end,float bias = .001);

struct Node {

	//Unique identifier
	int id;
	
	//Path info
	Tile* tile;
	Tile* start;
	Tile* end;
	
	//Bias
	//Values larger than default (.001) will allow for more exploration
	float default_bias = .001;
	float bias = .001;
	
	void setBias(float b) {
		this->bias = std::min(b,1.0f);
	}
	
	float gScore;
	float hScore;	
	
	inline float getX();
	inline float getY();

	float f() {
		return h(this->tile,this->start,this->end,this->bias) + this->gScore;
	}
		
	Node(Tile* t,Tile* s, Tile* e): tile(t), start(s), end(e) {
		
		this->id = ++node_count;
		
		this->gScore = 0xFFFFFF;
		this->hScore = h(tile,start,end);
		
	}
	
	Node() {}
	
};

//====================
//			THREADS
//====================

struct TBA_Thread {
	int id;
	int active = false;
	
	pthread_t thread;
	pthread_cond_t canUpdateClient = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t updateLock = PTHREAD_MUTEX_INITIALIZER;
	
	void client_thread_routine() {}
	
};

struct TBA_ThreadPool {
	
	int size;
	std::vector<TBA_Thread*> threads;
	
	TBA_ThreadPool(int numThreads): size(numThreads) {
		this->size = numThreads;
	}
	
	TBA_Thread* getThread(int i) {
		if(i >= this->threads.size()) {
			std::cout << "TBA_ThreadPool::getThread(int): Retrieving invalid object" << std::endl;
			return nullptr;
		}
		return this->threads.at(i);
	}
	
	bool full() {
		return this->activeThreads() == this->size;
	}
	
	int activeThreads() {
		int active = 0;
		for(int i=0;i<this->threads.size();i++) {
			if(this->threads.at(i)->active) active++;
		}
		return active;
	}

	bool initialize(void*(routine)(void*)) {
		for(int i=0;i<this->size;i++) {
			TBA_Thread *worker = new TBA_Thread();
			worker->id = i;
			if(pthread_create(&worker->thread,NULL,routine,worker) != 0) return false;
			this->threads.push_back(worker);
		}
		return true;
	}
	
};

struct TBA_ThreadHandler {

	TBA_ThreadPool pool;
	
	std::queue<GameObject> pending;
	

};

//====================
//			MISC/UTILITY
//====================

template <class T>
struct TBA_LimitedQueue {
	int size;
	
	std::queue<T> queue;
	
	TBA_LimitedQueue(int s): size(s) {
		
	}
	
	std::vector<T> get_elements() {
		std::vector<T> elements;
		std::queue<T> copy = this->queue;
		for(int i=0;i<copy.size();i++) {
			elements.push_back(copy.front());
			copy.pop();
		}
		return elements;
	}
	
	T pop() {
		return this->queue.front();
		this->queue.pop();
	}
	
	void push(T t) {
		this->queue.push(t);
		if(this->queue.size() == size) {
			this->queue.pop();
		}
	}
};

struct TBA_Interval {
	int last_valid_check;
	int interval;
	
	bool check() {
		auto now = SDL_GetTicks();
		if(this->last_valid_check + this->interval <= now) {
			this->last_valid_check = SDL_GetTicks();
			return true;
		}
		return false;
	}
	
	int remaining() {
		auto now = SDL_GetTicks();
		return this->last_valid_check + this->interval - now;
	}
	
	TBA_Interval() {}
	
	TBA_Interval(int i): interval(i) {
		this->last_valid_check = 0;
	}
};

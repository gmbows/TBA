#pragma once 

#include <map>
#include <utility>
#include <queue>
#include <vector>
#include <set>

#include "../tools/Utility.h"
/*
=========
  Overview
=========
The purpose of the Goal and Objective classes are to abstract complex goals and instructions into 
objects and functions. A distinction is made between "simple" and "complex" goals:
	A simple goal:
		-Has a single explicit list of objectives to be completed
	A complex goal:
		-Has multiple options for how to proceed, so a decision must be made

All simple goals have:
	-A list of preconditions that must first be met in order for that goal to be considered achievable
	-A list of objectives, that define what activities must be carried out to complete the goal
		-Which each in turn have a list of instruction functions that describe how to complete that objective


===================
  Parsing complex goals
===================

Because complex goals cannot themselves be acted upon, complex goals are "converted" into simple goals through a decision making process:
Complex goals are mapped to their options, which may themselves be simple or complex.
Until we've reached a point where we are deciding between one of several simple goals, carry out the following:
To go from a complex goal to a simple goal:
1. Determine the list of options (goals) to complete the complex goal
2. Eliminate unachievable goals by checking their preconditions
	-To eliminate complex goals, we must verify that none if its immediate options are achievable
		-This must be accomplished recursively by traversing the tree of options that it is upstream of
3. If we end up with multiple possible options, we must explicity define priority for each goal and use this priority to decide between them
	-Deciding between goals with undefined priorities will raise an error
4. If we decide upon a complex goal or only a complex goal remains, repeat

If the current goal's type_default is complex, we need to step through this decision process each time the goal is executed
However, we should only reset our progress (clear and initialize_goal) if we've decided on a different solution since the last time the goal was executed

==============
  Repeating goals
==============
There are some goals whose behavior is loop-like rather than a one-time completion
To serve this end, it's possible to have goals repeat upon completion rather than having them end after the final instruction
For example, the goal of "Finishing combat" is rather complex, and it involves:
	1. Choosing a target
	2. Killing that target
	3. Repeat

This looping behavior can be achieved by allowing objectives to return a repetition flag rather than returning completion flag
When a repeat flag is returned from executing an objective, the goal will reinitialize
*/

class Character;

//Precondition check function
typedef bool (*Precond)(Character*);

enum AIFlag: int {

	AI_GOAL_COMPLETE,
	AI_GOAL_INCOMPLETE,
	AI_GOAL_PRECOND_FAIL,
	AI_INSTRUCTION_COMPLETE,
	AI_INSTRUCTION_INCOMPLETE,
	AI_OBJECTIVE_COMPLETE,
	AI_OBJECTIVE_INCOMPLETE,
	
	AI_GOAL_REPEAT,
	
	AI_FATAL_ERROR,
	
};


//Instruction function
// Structure should be:
// -Returns false if not yet completed
// -Should have some checking interval like pathfinding
// -When returns true, remove from instruction queue
typedef AIFlag (*Instruction)(Character*);
// typedef std::pair<instructionFunction,int> Instruction;

enum GoalType: int {
	GOAL_IDLE =							1 << 0,
	GOAL_COMBAT  =					1 << 1,
	GOAL_COMBAT_KILL_ENEMY =	1 << 9,
	GOAL_TARGET_ENEMY = 			1 << 2,
	GOAL_COMBAT_USE_MELEE = 	1 << 3,
	GOAL_COMBAT_USE_RANGED = 	1 << 4,
	GOAL_FIND_AMMO = 				1 << 5,
	GOAL_WORK = 						1 << 6,
	GOAL_HEAL_SELF = 					1 << 7,
	GOAL_HEAL_SELF_POTION = 		1 << 8,
};

enum ObjectiveType: int {
	OBJECTIVE_IDLE,
	OBJECTIVE_TARGET_ENEMY,
	OBJECTIVE_ATTACK,
	OBJECTIVE_SEARCH_CONTAINERS_AMMO,
	OBJECTIVE_GOTO_WORK_TARGET,
	OBJECTIVE_WORK,
	OBJECTIVE_USE_HEALING_POTION,
};

struct Objective {
	
	int id;
	ObjectiveType type;
	std::queue<Instruction> instructions;
	bool inline isComplete() {return this->instructions.empty();}
	
	AIFlag execute(Character*);
	
	Objective(ObjectiveType t): type(t) {
		
	}
	
	Objective() {}
	
};

struct Goal {
	
	GoalType type;
	GoalType type_default;
	
	bool complex;
	
	//Vector of precondition check functions
	std::vector<Precond> preconditions;
	
	std::queue<Objective> objectives;
	
	bool inline isComplete() {return this->objectives.empty();}
	void initialize_type();
	void clear();

	//Returns flag describing exit conditions
	AIFlag execute(Character*);

	Goal(Character *c, GoalType t);

	~Goal() {
		// debug("Deleting goal");
		// delete this;
	}
};

inline int operator|(GoalType f1,GoalType f2) {
	return (int)f1 | (int)f2;
}
inline int operator&(GoalType f1,GoalType f2) {
	return (int)f1 & (int)f2;
}
inline bool operator==(GoalType f1,GoalType f2) {
	return (f1 ^ f2) == 0;
}
inline bool operator!=(GoalType f1,GoalType f2) {
	return (f1 ^ f2) > 0;
}


struct TBA_GoalQueue {
	std::priority_queue<Goal*> goals;
	
	int status = 0;
	
	bool hasGoal(GoalType t) {
		return (this->status & t) > 0;
	}
	
	inline bool hasGoals() { return !this->goals.empty(); }
	
	inline bool empty() { return this->goals.empty();}
	inline Goal* top() { return this->goals.top();}
	
	Goal* currentGoal() { 
		if(!this->hasGoals()) {
			// debug("Error (Character::currentGoal()): Returning nullpointer");
		}
		return this->goals.top();
	}

	void addGoal(Character *c,GoalType t) {
		// debug("Adding goal "+std::to_string(t));
	if(!this->hasGoal(t)) {
			this->status = (this->status | t);
			this->goals.push(new Goal(c,t));	
		} else {
			debug("Error (addGoal()): Skipping existing goal "+std::to_string(t));
			// debug(this->goals.size());
		}
	}
	
	Goal* pop() {
		Goal *g = this->goals.top();
		this->goals.pop();
		this->status = (this->status & ~g->type_default);
		return g;
	}
	
	bool remove(GoalType g) {
		std::vector<Goal*> tgoals;
		while(!this->goals.empty()) {
			tgoals.push_back(this->goals.top());
			this->goals.pop();
		}
		for(int i=0;i<tgoals.size();i++) {
			if(tgoals.at(i)->type != g) {
				this->goals.push(tgoals.at(i));
			}
		}
		return true;
	}
	
	std::vector<GoalType> get_elements() {
		std::vector<GoalType> tgoals;
		std::priority_queue<Goal*> g = this->goals;
		while(!g.empty()) {
			tgoals.push_back(g.top()->type);
			g.pop();
		}
		return tgoals;
	}
	
	TBA_GoalQueue() {
		
	}
};

int determine_priority(Character*, GoalType);

//Decides on a single course of action based on available options
GoalType decide(Character*,const std::vector<GoalType>&);

extern std::map<GoalType,std::pair<std::vector<Precond>,std::vector<std::pair<ObjectiveType,std::vector<Instruction>>>>> goalMap;
extern std::map<GoalType,std::vector<GoalType>> decisionMap;
#pragma once 

#include <map>
#include <utility>
#include <queue>
#include <vector>
#include <set>

#include "../tools/Utility.h"

class Character;

//Precondition check function
typedef bool (*Precond)(Character*);

//Instruction function
// Structure should be:
// -Returns false if not yet completed
// -Should have some checking interval like pathfinding
// -When returns true, remove from instruction queue
typedef bool (*instructionFunction)(Character*,int);
typedef std::pair<instructionFunction,int> Instruction;

/*
Top level goals are most abstracted objectives like "survive" or "kill enemy" or "find food"
Goals are assigned in behavior to avoid iterating through all available goals and choosing them based on existing conditions
Each goal has sub-goals with preconditions such as "has containers nearby" or "has enemy"

MOTIVE_FIND_FOOD:
	GOAL_PICK_BERRIES:
		PRECOND_HAS_BUSH_NEARBY
		OBJECTIVE_MOVE_TO_BUSH
		OBJECTIVE_PICK_BERRIES
	GOAL_HUNT:
		PRECOND_ANIMALS_NEARBY
		PRECOND_CAN_FIGHT
			PRECOND_HAS_MELEE_WEAPON | PRECOND_HAS_RANGED_WEAPON
				PRECOND_HAS_AMMO

MOTIVE_FIND_FOOD has implicit preconditions that are defined in Behavior.cpp
It will be assigned to a character that meets these conditions
*/

enum GoalFlag: int {
	TBA_GOAL_COMPLETE,
	TBA_GOAL_INCOMPLETE,
	TBA_GOAL_PRECOND_FAIL,
};

enum GoalType: int {
	GOAL_ATTACK_TARGET,
	GOAL_USE_MELEE,
	GOAL_USE_RANGED,
	GOAL_FIND_AMMO,
	GOAL_WORK,
};

enum ObjectiveType: int {
	OBJECTIVE_FIRE_WEAPON,
	OBJECTIVE_SEARCH_CONTAINERS_AMMO,
	OBJECTIVE_GOTO_WORK_TARGET,
	OBJECTIVE_WORK,
};

struct Objective {
	
	int id;
	ObjectiveType type;
	std::queue<Instruction> instructions;
	bool inline isComplete() {return this->instructions.empty();}
	
	bool execute(Character*);
	
	Objective(ObjectiveType t): type(t) {
		
	}
	
};

struct Goal {
	
	GoalType type;
	
	//Vector of precondition check functions
	std::vector<Precond> preconditions;
	
	std::queue<Objective*> objectives;
	
	bool inline isComplete() {return this->objectives.empty();}
	
	//Returns false if this goal:
	// -Was completed
	// -Can no longer be completed
	//  Should be deleted in such a case
	GoalFlag execute(Character*);
	
	Goal(GoalType t);
	
	~Goal() {
		// debug("Deleting goal");
		// delete this;
	}
};


extern std::map<GoalType,std::pair<std::vector<Precond>,std::vector<std::pair<ObjectiveType,std::vector<Instruction>>>>> goalMap;


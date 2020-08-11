#include "AI.h"
#include "AI_Defs.h"

#include "Item.h"
#include "Character.h"

#include "../tools/Utility.h"

Goal::Goal(GoalType t): type(t) {
	
	if(goalMap.find(t) == goalMap.end()) {
		debug("ERROR (Goal::Goal()): Invalid goal type");
		return;
	}
	
	ObjectiveType o;
	
	Objective *oObject;
	
	this->preconditions = goalMap.at(this->type).first;
	for(int j=0;j<goalMap.at(this->type).second.size();j++) {
		//Get objectives from map and the instructions to carry them out
		oObject = new Objective(goalMap.at(this->type).second.at(j).first);
		for(int i=0;i<goalMap.at(this->type).second.at(j).second.size();i++) {
			oObject->instructions.push(goalMap.at(this->type).second.at(j).second.at(i));
		}
		this->objectives.push(oObject);
	}

	// delete oObject;
	
}

GoalFlag Goal::execute(Character* c) {
	//This function should return false when this goal can be discarded
	for(int i=0;i<this->preconditions.size();i++) {
		if(!this->preconditions.at(i)(c)) {
			// debug(c->getEntityName()+" failed precondition check "+std::to_string(i));
			return TBA_GOAL_PRECOND_FAIL;
		}
	}
	
	if(this->objectives.front()->execute(c)) {
		//If executing the current objective returns true then it is complete
		this->objectives.pop();
	}
	
	//Will return true when complete
	if(this->isComplete()) {
		return TBA_GOAL_COMPLETE;
	} else {
		return TBA_GOAL_INCOMPLETE;
	}
}

bool Objective::execute(Character* c) {
	
	//Execute current instruction
	instructionFunction func = this->instructions.front().first;
	int aux = this->instructions.front().second;
	
	//Instructions will return true if completed
	if(func(c,aux)) {
		this->instructions.pop();
	}
	return this->isComplete();
}


bool operator<(Goal g1, Goal g2) {
	return (int)g1.type > (int)g2.type;
}

std::map<GoalType,std::pair<std::vector<Precond>,std::vector<std::pair<ObjectiveType,std::vector<Instruction>>>>> goalMap = {
		
			{GOAL_USE_RANGED,
				{
					{precondHasAmmo}, {
						{
							OBJECTIVE_FIRE_WEAPON, {{instructionFireWeapon,0}}
						},
					}
				}
			},

			{GOAL_FIND_AMMO,
				{
					{}, {
						{
							OBJECTIVE_SEARCH_CONTAINERS_AMMO, {{instructionSearchContainers,(int)I_AMMO}}
						},
					}
				}
			},
			
			{GOAL_WORK,
				{
					{precondHasWorkTarget,precondCanReachWorkTarget}, {
						{OBJECTIVE_GOTO_WORK_TARGET, {{instructionGotoWorkTarget,0}}},
						{OBJECTIVE_WORK, {{instructionWork,0}}},
					}
				}
			},
};
#include "AI.h"
#include "AI_Defs.h"

#include "Item.h"
#include "Character.h"

#include "../tools/Utility.h"

bool is_complex(GoalType type) {
	return decisionMap.find(type) != decisionMap.end();
}

std::vector<GoalType> determine_all_options(GoalType t) {
	//Takes complex goal, determines all possible simple goals that it can lead to
	std::vector<GoalType> v;
	std::vector<GoalType> temp;
	if(is_complex(t)) {
		for(int i=0;i<decisionMap.at(t).size();i++) {
			if(!is_complex(decisionMap.at(t).at(i))) {
				v.push_back(decisionMap.at(t).at(i));
			} else {
				temp = determine_all_options(decisionMap.at(t).at(i));
				for(int j=0;j<temp.size();j++) {
					v.push_back(temp.at(j));
				}
			}
		}
		return v;
	} else {
		return {t};
	}
}

//Verify that all of this goal's preconditions are met
bool check_preconditions(Character *c, GoalType t) {
	// debug(c->getEntityName()+ ": Executing goal "+std::to_string(t));
	if(goalMap.find(t) == goalMap.end()) {
		if(decisionMap.find(t) == decisionMap.end()) {
			debug("ERROR (check_preconditions()): Invalid goal type "+std::to_string((int)t));
			return false;
		} else {
			//This is a complex goal
			// debug("Total options for goal "+std::to_string(t)+": "+std::to_string(determine_all_options(t).size()));
			for(int i=0;i<decisionMap.at(t).size();i++) {
				//Return true if we pass the preconds for at least one option, will recurse further if necessary
				//This works because all complex goals resolve eventually to simple goals
				if(check_preconditions(c,decisionMap.at(t).at(i))) return true;
			}
			return false;
		}
	}
	
	std::vector<Precond> p = goalMap.at(t).first;
	
	for(int i=0;i<p.size();i++) {
		if(!p.at(i)(c)) {
			if(c->getName() == "Archer" or true) {
				debug(c->getEntityName()+" failed precondition check "+std::to_string(i)+" for goal "+std::to_string(t));
			}
			return false;
		}
	}
	return true;
}

std::vector<GoalType> get_valid_options(Character *c, std::vector<GoalType> v) {
	std::vector<GoalType> valid;
	for(int i=0;i<v.size();i++) {
		if(check_preconditions(c,v.at(i))) {
			valid.push_back(v.at(i));
		} else {
			// debug(c->getEntityName()+" failed preconditions for goal "+std::to_string(v.at(i)));
		}
	}
	return valid;
}



void Goal::clear() {
	while(this->objectives.size() > 0) {
		std::queue<Instruction>().swap(this->objectives.front().instructions);
		this->objectives.pop();
	}
	std::queue<Objective>().swap(this->objectives);
	std::vector<Precond>().swap(this->preconditions);
}

void Goal::initialize_type() {
	
	this->clear();
	
	// this->type_default = this->type;
	
	// debug("Initializing goal");
	
	if(goalMap.find(this->type) == goalMap.end()) {
		debug("ERROR (Goal::initialize_type()): Invalid goal type "+std::to_string(this->type));
		return;
	}

	Objective oObject;
	this->preconditions = goalMap.at(this->type).first;
	for(int j=0;j<goalMap.at(this->type).second.size();j++) {
		//Get objectives from map and the instructions to carry them out
		oObject = Objective(goalMap.at(this->type).second.at(j).first);
		for(int i=0;i<goalMap.at(this->type).second.at(j).second.size();i++) {
			oObject.instructions.push(goalMap.at(this->type).second.at(j).second.at(i));
		}
		this->objectives.push(oObject);
	}	
}

Goal::Goal(Character *c,GoalType t): type_default(t), type(t) {

	if(goalMap.find(t) == goalMap.end()) {
		if(decisionMap.find(t) == decisionMap.end()) {
			debug("ERROR (Goal::Goal()): Invalid goal type");
			return;
		} else {
			this->complex = true;
		}
	} else {
		this->complex = false;
		this->initialize_type();
	}
	
}

bool operator<(Goal g1, Goal g2) {
	return (int)g1.type < (int)g2.type;
}

AIFlag Goal::execute(Character* c) {
	
	// debug(c->getEntityName()+ ": Executing goal "+std::to_string(this->type));
	
	//If this goal was instantiated with a type with multiple options
	//Decide between options whose preconditions we pass then execute
	// this->type = this->type_default;
	GoalType testGoal;

	// debug("Continuing");
	if(is_complex(this->type_default) or c->checkAI.check()) {
		if(is_complex(this->type)) {
			testGoal = this->type_default;
			while(is_complex(testGoal)) {
				std::vector<GoalType> valid_options = get_valid_options(c,decisionMap.at(testGoal));
				//We have more than one valid option so decide between them
				if(valid_options.size() > 1) {
					//Decide between options
					testGoal = decide(c,valid_options);
				} else if(valid_options.size() == 1) {
					//Although we had multiple options only one is currently valid/achievable
					testGoal = valid_options.at(0);
				} else {
					//Failed preconditions for all possible courses of action
					return AI_GOAL_PRECOND_FAIL;
				}
			}
			if(testGoal != this->type) {
				this->type = testGoal;
				this->initialize_type();
			}
		}
	} else {
		//Check the preconditions of our simple goal
		if(!check_preconditions(c,this->type)) return AI_GOAL_PRECOND_FAIL;
	}
	
	// debug(c->getEntityName()+ ": Executing goal "+std::to_string(this->type));
	
	// if(this->type == GOAL_USE_RANGED) debug("IT worked");
	
	// debug(c->getEntityName()+" is proceeding with goal "+std::to_string(this->type));

	AIFlag exitValue = this->objectives.front().execute(c);
	
	switch(exitValue) {
		
		//If we completed an objective,
		// Check if it was the last objective
		case AI_OBJECTIVE_COMPLETE:
			this->objectives.pop();
			if(this->isComplete()) {
				return AI_GOAL_COMPLETE;
			} else {
				return AI_GOAL_INCOMPLETE;
			}
			
		//Passthrough
		case AI_OBJECTIVE_INCOMPLETE:
			return AI_GOAL_INCOMPLETE;
		case AI_FATAL_ERROR:
			return AI_FATAL_ERROR;
		case AI_GOAL_REPEAT:
			this->initialize_type();
			return AI_GOAL_INCOMPLETE;
		default:
			debug("Error: Goal::execute()): Received unexpected objective exit value");
			return AI_FATAL_ERROR;
	}
}

AIFlag Objective::execute(Character* c) {
	
	//Execute current instruction
	Instruction func = this->instructions.front();
	
	//Instructions will return flags defining exit value
	AIFlag exitValue = func(c);
	
	switch(exitValue) {
		case AI_INSTRUCTION_COMPLETE:
			// debug(c->getEntityName()+" completed instruction "+std::to_string(this->instructions.size()));
			this->instructions.pop();
			if(this->isComplete()) {
				// debug(c->getEntityName()+" completed objective");
				return AI_OBJECTIVE_COMPLETE;
			} else {
				return AI_OBJECTIVE_INCOMPLETE;
			}
			
		//Passthrough
		case AI_INSTRUCTION_INCOMPLETE:
			return AI_OBJECTIVE_INCOMPLETE;
		case AI_FATAL_ERROR:
			return AI_FATAL_ERROR;
		case AI_GOAL_REPEAT:
			return AI_GOAL_REPEAT;
		default:
			//If case defaults, we received a chain goal
			// Return to goal to be reinitialized with the chain
			debug("Error: Objective::execute()): Received unexpected objective exit value");
			return AI_FATAL_ERROR;
	}

}

int determine_priority(Character* c, GoalType t) {
	//Placeholders for now
	switch(t) {
		case GOAL_COMBAT_USE_MELEE:
			return 0;
		case GOAL_COMBAT_USE_RANGED:
			return 1;
		case GOAL_TARGET_ENEMY:
			return 3;
		case GOAL_COMBAT_KILL_ENEMY:
			return 2;
		default:
			debug("Warning (determine_priority()): Returning undefined priority for goal "+std::to_string(t));
			return -1;
	}
}

//Decides on a single course of action based on available options
GoalType decide(Character *c,const std::vector<GoalType> &goals) {
	GoalType best_option;
	int highest_priority = -1;
	int priority;
	for(int i=0;i<goals.size();i++) {
		priority = determine_priority(c,goals.at(i));
		if(priority > highest_priority) {
			highest_priority = priority;
			best_option = goals.at(i);
		}
	}
	return best_option;
}

std::map<GoalType,std::pair<std::vector<Precond>,std::vector<std::pair<ObjectiveType,std::vector<Instruction>>>>> goalMap = {
	
			{GOAL_TARGET_ENEMY,
				{
					{precondNoLivingTarget,precondHasNearbyEnemy}, {
						{
							OBJECTIVE_TARGET_ENEMY, {instructionTargetNearestEnemy}
						},
					}
				}
			},


			{GOAL_COMBAT_USE_RANGED,
				{
					{precondHasNearbyVisibleEnemy,precondHasRangedWeapon,precondHasAmmo}, {
						{OBJECTIVE_TARGET_ENEMY, {instructionTargetNearestEnemy}},
						{OBJECTIVE_ATTACK, {instructionEquipRangedWeapon,instructionLookAtTarget,instructionStop,instructionKillTarget}}
					}
				}
			},
			
			{GOAL_COMBAT_USE_MELEE,
				{
					{precondHasNearbyReachableEnemy,precondHasMeleeWeapon}, {
						{OBJECTIVE_TARGET_ENEMY, {instructionTargetNearestEnemy}},
						{OBJECTIVE_ATTACK, {instructionEquipMeleeWeapon,instructionGotoTarget,instructionKillTarget}},
					}
				}
			},

			{GOAL_FIND_AMMO,
				{
					{}, {
						{
							OBJECTIVE_SEARCH_CONTAINERS_AMMO, {instructionSearchContainers}
						},
					}
				}
			},
			
			{GOAL_WORK,
				{
					{precondHasWorkTarget,precondCanReachWorkTarget}, {
						{OBJECTIVE_GOTO_WORK_TARGET, {instructionGotoWorkTarget}},
						{OBJECTIVE_WORK, {instructionWork}},
					}
				}
			},
			
			{GOAL_HEAL_SELF_POTION,
				{
					{precondHasHealingPotion}, {
						{OBJECTIVE_USE_HEALING_POTION,{instructionUseHealingPotion}},
					}
				}
			},
			
			{GOAL_IDLE,
				{
					{}, {
						{OBJECTIVE_IDLE,{instructionIdle}},
					}
				}
			},
			
};

std::map<GoalType,std::vector<GoalType>> decisionMap = {
	{GOAL_COMBAT,
		{GOAL_COMBAT_KILL_ENEMY}
	},
	{GOAL_COMBAT_KILL_ENEMY,
		{GOAL_COMBAT_USE_MELEE,GOAL_COMBAT_USE_RANGED}
	},
	{GOAL_HEAL_SELF,
		{GOAL_HEAL_SELF_POTION}
	}
};

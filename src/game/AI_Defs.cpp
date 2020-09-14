#include "AI_Defs.h"


#include "Character.h"
#include "Squad.h"
#include "Item.h"
#include "Projectile.h"
#include "CommandUtils.h"
#include "ResourceNode.h"

#include "../tools/Algorithm.h"
#include "../tools/Error.h"

//=======================================
//	-Preconds should return true if satisfied

//====================
//			PRECONDS
//====================

bool precondHasAmmo(Character *c) {
	// if(!c->hasPrimary()) return false;
	
	// if(!c->equipment->getSlot(EQUIP_PRIMARY)->hasType(I_WEAPON_RANGED)) return false;
	//Check if primary is one of these types
	std::vector<ItemType> types = {I_WEAPON_BOW};
	ItemType t;
	for(int i=0;i<types.size();i++) {
		t = types.at(i);
		// if(c->equipment->getSlot(EQUIP_PRIMARY)->hasType(t)) {
			return c->hasAmmo(t);
		// }
	}
	return false;
}

bool precondHasWorkTarget(Character *c) {
	return c->hasWorkTarget();
}

bool precondHasTarget(Character *c) {
	return c->hasTarget();
}

bool precondNoLivingTarget(Character *c) {
	return !precondHasLivingTarget(c);
}

bool precondHasLivingTarget(Character *c) {
	return (c->hasTarget() and c->getCharTarget()->isAlive());
}

bool precondCanReachWorkTarget(Character *c) {
	return c->canReach(c->getWorkTarget());
}

bool precondCanReachTarget(Character *c) {
	return c->canReach(c->getTarget());
}

bool precondCanSeeTarget(Character *c) {
	return c->canSee(c->getTarget());
}

bool precondHasMeleeWeapon(Character *c) {
	for(std::map<int,unsigned int>::iterator it = c->inventory->manifest.begin();it != c->inventory->manifest.end();it++) {
		if(ItemUtils::hasType(it->first,I_WEAPON_MELEE | I_WEAPON)) {
			return true;
		}
	}
	return false;
}

bool precondHasRangedWeapon(Character *c) {
	for(std::map<int,unsigned int>::iterator it = c->inventory->manifest.begin();it != c->inventory->manifest.end();it++) {
		if(ItemUtils::hasType(it->first,I_WEAPON_RANGED) and ItemUtils::hasType(it->first,I_WEAPON)) {
			return true;
		}
	}
	return false;
}

bool precondHasNearbyEnemy(Character *c) {
	//placeholder
	std::vector<Character*> targets = c->getCharactersInRadius();
	if(c->hasSquad()) {
		for(int i=0;i<targets.size();i++) {
			if(!c->squad->isMember(targets.at(i))) {
				return true;
			}
		}
	}
	return targets.size() > 0;
}

bool precondHasNearbyVisibleEnemy(Character *c) {
	//placeholder
	std::vector<Character*> targets = c->getCharactersInRadius();
	for(int i=0;i<targets.size();i++) {
		if((c->hasSquad() and !c->squad->isMember(targets.at(i))) or !c->hasSquad()) {
			if(c->canSee(targets.at(i))) {
				return true;
			}
		}
	}

	return false;
}

bool precondHasNearbyReachableEnemy(Character *c) {
	//placeholder
	std::vector<Character*> targets = c->getCharactersInRadius();
	for(int i=0;i<targets.size();i++) {
		if((c->hasSquad() and !c->squad->isMember(targets.at(i))) or !c->hasSquad()) {
			if(c->canReach(targets.at(i))) {
				return true;
			}
		}
	}

	return false;
}

bool precondHasHealingPotion(Character *c) {
	for(std::map<int,unsigned int>::iterator it = c->inventory->manifest.begin();it != c->inventory->manifest.end();it++) {
		if(it->first == 3) {
			return true;
		}
	}
	return false;
}

//====================
//		   INSTRUCTIONS
//====================
//-Instructions should return AI_INSTRUCTION_COMPLETE when complete

AIFlag instructionEquipRangedWeapon(Character *c) {
	
	//We should eventually only count ranged weapons if we also have ammo for them
	
	if(c->equipment->hasEquipped(EQUIP_PRIMARY)) {
		if(c->equipment->getSlot(EQUIP_PRIMARY)->hasType(I_WEAPON_RANGED)) return AI_INSTRUCTION_COMPLETE;
	}

	int first_ranged_weapon_id = -1;
	for(std::map<int,unsigned int>::iterator it = c->inventory->manifest.begin();it != c->inventory->manifest.end();it++) {
		if(ItemUtils::hasType(it->first,I_WEAPON_RANGED) and ItemUtils::hasType(it->first,I_WEAPON)) {
			first_ranged_weapon_id = it->first;
			c->equip(c->inventory->get(c->inventory->find(first_ranged_weapon_id)));
			// debug("Equipping "+c->inventory->get(c->inventory->find(first_ranged_weapon_id))->getName());
			return AI_INSTRUCTION_COMPLETE;
		}
	}
	// debug("Could not equip");
	return AI_FATAL_ERROR;
}

AIFlag instructionEquipMeleeWeapon(Character *c) {
	
	if(c->equipment->hasEquipped(EQUIP_PRIMARY)) {
		if(c->equipment->getSlot(EQUIP_PRIMARY)->hasType(I_WEAPON_MELEE)) return AI_INSTRUCTION_COMPLETE;
	}
	
	int first_melee_weapon_id = -1;
	for(std::map<int,unsigned int>::iterator it = c->inventory->manifest.begin();it != c->inventory->manifest.end();it++) {
		if(ItemUtils::hasType(it->first,I_WEAPON_MELEE)) {
			first_melee_weapon_id = it->first;
			c->equip(c->inventory->get(c->inventory->find(first_melee_weapon_id)));
			return AI_INSTRUCTION_COMPLETE;
		}
	}
	return AI_FATAL_ERROR;
}

AIFlag instructionKillTarget(Character *c) {
		switch(c->getAttackStatus()) {
		//Character is in combat but not ready to attack
		case ATK_NOT_READY:
			// this->addStatus(STATUS_COMBAT);
			return AI_INSTRUCTION_INCOMPLETE;
		//Character is currently attacking with weapon
		//Wait for attack to connect
		case ATK_STATUS_ATTACKING:
			c->addStatus(STATUS_ATTACK);
			return AI_INSTRUCTION_INCOMPLETE;
		//Swing makes contact, send attack and calculate damage
		case ATK_COMPLETE:
			c->removeStatus(STATUS_ATTACK);
			break;
	}

	if(!c->hasTarget()) {
		// debug("Error: (instructionKillEnemy()): Precond returned false-positive - this shouldn't happen");
		return AI_FATAL_ERROR;
	}
	
	c->sendAttack(c->getTarget());
	
	if(!c->getCharTarget()->isAlive() or !c->getCharTarget()->hasStatus(STATUS_COMBAT)) {
		c->setTarget(nullptr);
		return AI_GOAL_REPEAT;
	}
	return AI_INSTRUCTION_INCOMPLETE;
}

AIFlag instructionSearchContainers(Character *c) {
	ItemType type = I_AMMO;
	float searchRadius = 10;
	std::vector<GameObject*> containers = c->getObjectsInRadius(OBJ_CONTAINER);
	if(containers.size() == 0) return AI_FATAL_ERROR;
	
	GameObject *closest = c->selectClosestObject(containers);
	
	if(!c->goTo(closest,true)) debug(c->getEntityName()+"Can't reach closest container "+closest->getEntityName());
	
	if(!c->canInteract(closest)) {
		return AI_INSTRUCTION_INCOMPLETE;
		//We can reset delay here
	}
	
	int ammo_id = 9;
	
	// if(closest->getInventory()->find(ammo_id) == -1) {
		// debug("No arrows found in closest container");
		// return true;
	// }
	
	closest->getInventory()->add(ammo_id,41);
	int taken = moveItems(40,ammo_id,closest->getInventory(),c->inventory);
	// debug(c->getName()+" took "+std::to_string(taken)+" arrows from "+closest->getName());
	return AI_INSTRUCTION_COMPLETE;
	
}

AIFlag instructionGotoWorkTarget(Character *c) {
	//If work target is out of range, go to it
	if(!c->workTargetInRange()) {
		if(!c->goTo(c->getWorkTarget(),true)) {
			debug("Error: "+c->getEntityName()+" cannot reach work target");
		}
		return AI_INSTRUCTION_INCOMPLETE;
	}
	return AI_INSTRUCTION_COMPLETE;
}

AIFlag instructionGotoTarget(Character *c) {
	
	//If target is out of range, go to it
	if(c->targetInRange()) return AI_INSTRUCTION_COMPLETE;
	
	if(!c->goTo(c->getTarget(),false)) {
		debug("Error: "+c->getEntityName()+" cannot reach target");
		return AI_FATAL_ERROR;
	}
	return AI_INSTRUCTION_INCOMPLETE;
}

AIFlag instructionLookAtTarget(Character *c) {
	
	return AI_INSTRUCTION_COMPLETE;
}

AIFlag instructionWork(Character *c) {
		// debug(c->getEntityName()+" is in working range");
		c->targetPath.clear();
		c->removeStatus(STATUS_TRAVEL);
		c->autoMove = false;
		c->move_forward = false;
		if(!c->hasStatus(STATUS_COMBAT)) {
			c->workTarget->getAsResource()->work();
		}
		return AI_INSTRUCTION_INCOMPLETE;
}

AIFlag instructionTargetNearestEnemy(Character *c) {
	
	std::vector<Character*> targets = c->getCharactersInRadius();
		
	if(targets.size() == 0) {
		TBA_throw(WARN_DEFAULT,__PRETTY_FUNCTION__,"Precondition check returned a false-positive");
		return AI_FATAL_ERROR;
	}
	
	float smallest_dist = 0xFFFF;
	float tdist;
	Character* nearest = nullptr;
	
	for(int i=0;i<targets.size();i++) {
		if(!targets.at(i)->isAlive()) continue;
		if((c->hasSquad() and !c->squad->isMember(targets.at(i))) or !c->hasSquad()) {
			tdist = dist(targets.at(i)->getLocation(),c->getLocation());
			if(tdist < smallest_dist) {
				nearest = targets.at(i);
				smallest_dist = tdist;
			}
		}
	}
	c->setTarget(nearest);
	return AI_INSTRUCTION_COMPLETE;
}

AIFlag instructionTargetNearestVisibleEnemy(Character *c) {
	//placeholder
	std::vector<Character*> targets = c->getCharactersInRadius();
		
	if(targets.size() == 0) {
		TBA_throw(WARN_DEFAULT,__PRETTY_FUNCTION__,"Precondition check returned a false-positive");
		return AI_FATAL_ERROR;
	}
	
	float smallest_dist = 0xFFFF;
	float tdist;
	Character* nearest = nullptr;
	
	for(int i=0;i<targets.size();i++) {
		if(!targets.at(i)->isAlive()) continue;
		if(!c->canSee(targets.at(i))) continue;
		if((c->hasSquad() and !c->squad->isMember(targets.at(i))) or !c->hasSquad()) {
			tdist = dist(targets.at(i)->getLocation(),c->getLocation());
			if(tdist < smallest_dist) {
				nearest = targets.at(i);
				smallest_dist = tdist;
			}
		}
	}
	c->setTarget(nearest);
	return AI_INSTRUCTION_COMPLETE;
}

AIFlag instructionTargetEnemy(Character *c) {
	std::vector<Character*> targets = c->getCharactersInRadius();
	if(c->hasSquad()) {
		for(int i=0;i<targets.size();i++) {
			if(!c->squad->isMember(targets.at(i))) {
				c->setTarget(targets.at(i));
				return AI_INSTRUCTION_COMPLETE;
			}
		}
	}
	//Targets is non-empty
	if(targets.size() > 0) {
		c->setTarget(targets.at(0));
		return AI_INSTRUCTION_COMPLETE;
	} else {
		TBA_throw(WARN_DEFAULT,__PRETTY_FUNCTION__,"Precondition check returned a false-positive");
		return AI_FATAL_ERROR;
	}
	
}

AIFlag instructionStop(Character *c) {
	bool stopped = c->stop();
	return AI_INSTRUCTION_COMPLETE;
	return (stopped)? AI_INSTRUCTION_COMPLETE : AI_INSTRUCTION_INCOMPLETE;
}

AIFlag instructionUseHealingPotion(Character *c) {
	
	
	
	int healing_potion_id = 3;
	int healing_potion_index = c->inventory->find(healing_potion_id);
	
	if(healing_potion_index == -1) {
		debug("Error: (instructionUseHealingPotion()): Precond returned false-positive - this shouldn't happen");
		return AI_FATAL_ERROR;
	}

	Item* potion = c->inventory->get(healing_potion_index);
	c->consume(potion);
	return AI_INSTRUCTION_COMPLETE;
}

AIFlag instructionIdle(Character *c) {
	//Sleep this thread until we decide again
	c->checkAI.check();
	int remaining = c->checkAI.remaining();
		// debug(c->getEntityName()+" is idle for the next "+std::to_string(remaining)+"ms.");
	if(remaining > 0) {
		// std::this_thread::sleep_for(std::chrono::milliseconds(remaining));
	}
	return AI_INSTRUCTION_INCOMPLETE;
}
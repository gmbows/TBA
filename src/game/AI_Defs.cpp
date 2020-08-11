#include "AI_Defs.h"


#include "Character.h"
#include "Item.h"
#include "Projectile.h"
#include "CommandUtils.h"
#include "ResourceNode.h"

#include "../tools/Algorithm.h"

//=======================================
//	-Instructions should return true when complete
//	-Preconds should return true if satisfied


bool precondHasAmmo(Character *c) {
	if(!c->hasPrimary()) return false;
	if(!c->equipment->getSlot(EQUIP_PRIMARY)->hasType(I_WEAPON_RANGED)) return false;
	
	//Check if primary is one of these types
	std::vector<ItemType> types = {I_WEAPON_BOW};
	ItemType t;
	for(int i=0;i<types.size();i++) {
		t = types.at(i);
		if(c->equipment->getSlot(EQUIP_PRIMARY)->hasType(t)) {
			return c->hasAmmo(I_WEAPON_BOW);
		}
	}
	return false;
}

bool precondHasWorkTarget(Character *c) {
	return c->hasWorkTarget();
}

bool precondCanReachWorkTarget(Character *c) {
	return c->canReach(c->getWorkTarget());
}

bool instructionFireWeapon(Character *c,int aux) {
	float damage = c->getAttackDamage();
	float inaccuracy = 12;
	inaccuracy = (-(inaccuracy/2)+rand()%(int)inaccuracy);
	new Projectile(c,c->getLocation(),damage,(inaccuracy+c->viewAng)*CONV_DEGREES,.5);
	return true;
}

bool instructionSearchContainers(Character *c,int aux) {
	ItemType type = (ItemType)aux;
	float searchRadius = 10;
	std::vector<GameObject*> containers = c->getObjectsInRadius(OBJ_CONTAINER);
	if(containers.size() == 0) return false;
	
	GameObject *closest = c->selectClosestObject(containers);
	
	if(!c->goTo(closest,true)) debug(c->getEntityName()+"Can't reach closest container "+closest->getEntityName());
	
	if(!c->canInteract(closest)) {
		return false;
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
	c->removeStatus(STATUS_NO_AMMO);
	return true;
	
}

bool instructionGotoWorkTarget(Character *c,int aux) {
	//If work target is out of range, go to it
	if(!c->workTargetInRange()) {
		if(!c->goTo(c->getWorkTarget(),true)) {
			debug("Error: "+c->getEntityName()+" cannot reach work target");
		}
		return false;
	}
	return true;
}

bool instructionWork(Character *c,int aux) {
		// debug(c->getEntityName()+" is in working range");
		c->targetPath.clear();
		c->removeStatus(STATUS_TRAVEL);
		c->autoMove = false;
		c->move_forward = false;
		if(!c->hasStatus(STATUS_COMBAT)) {
			c->workTarget->getAsResource()->work();
		}
		return false;
}
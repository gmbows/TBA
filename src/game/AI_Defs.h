#pragma once

#include "AI.h"

class Character;


bool precondHasRangedWeapon(Character*);
bool precondHasMeleeWeapon(Character*);
bool precondHasAmmo(Character*);
bool precondHasWorkTarget(Character*);
bool precondHasTarget(Character*);
bool precondNoLivingTarget(Character*);
bool precondHasLivingTarget(Character*);
bool precondCanReachWorkTarget(Character*);
bool precondCanReachTarget(Character*);
bool precondCanSeeTarget(Character*);
bool precondHasNearbyEnemy(Character*);
bool precondHasNearbyVisibleEnemy(Character*);
bool precondHasNearbyReachableEnemy(Character*);
bool precondHasHealingPotion(Character*);

AIFlag instructionEquipRangedWeapon(Character*);
AIFlag instructionEquipMeleeWeapon(Character*);
AIFlag instructionFireWeapon(Character*);
AIFlag instructionKillTarget(Character*);
AIFlag instructionLookAtTarget(Character*);
AIFlag instructionSearchContainers(Character*);
AIFlag instructionWork(Character*);
AIFlag instructionGotoWorkTarget(Character*);
AIFlag instructionGotoTarget(Character*);
AIFlag instructionTargetEnemy(Character*);
AIFlag instructionTargetNearestEnemy(Character*);
AIFlag instructionTargetNearestVisibleEnemy(Character*);
AIFlag instructionStop(Character*);
AIFlag instructionUseHealingPotion(Character*);
AIFlag instructionIdle(Character*);
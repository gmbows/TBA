#pragma once

class Character;


bool precondHasAmmo(Character*);
bool precondHasWorkTarget(Character*);
bool precondCanReachWorkTarget(Character*);

bool instructionFireWeapon(Character*,int);
bool instructionSearchContainers(Character*,int);
bool instructionWork(Character*,int);
bool instructionGotoWorkTarget(Character*,int);

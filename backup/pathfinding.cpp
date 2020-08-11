bool Character::generatePathTo(float tx, float ty, bool adjacent) {
	if(TBAGame->logicTicks < this->lastPathCheck+TBAGame->pathCheckInterval) return true;
	// if(this->targetPath.size() > 0) return false;
	float g,h,f;
	float tg,th;
	
	Tile* targetTile = TBAGame->gameWorld->getTileAt(tx,ty);
	Tile *thisTile;
	Tile *currentBestTile = this->location;
	float currentBestF;
	
	float startingDist = dist({this->location->x,this->location->y},{tx,ty});

	std::vector<Tile*> bestPath;
	
	std::vector<Tile*> previousBests;

	thisTile = this->location;
	
	float tileX = this->location->x;
	float tileY = -this->location->y;
	
	int iter = 0;
	
	if(targetTile->isPassable() == false and adjacent == false) {
		debug(this->getName()+" cannot generate path to impassable tile "+std::to_string((int)tx)+", "+std::to_string((int)ty));
		this->lastPathCheck = TBAGame->logicTicks;
		return false;
	}
	
	while( !(tileX == tx and tileY == ty)) {
		
		currentBestF = 0xFF;
	
		for(int i=-1;i<=1;i++) {
			for(int j=-1;j<=1;j++) {
				if(j==0 and i==0) continue;
				if(bestPath.size() > 0) if(TBAGame->gameWorld->isCorner(TBAGame->gameWorld->getTileAt(tileX+j,tileY+i),bestPath.at(bestPath.size()-1))) continue;
				if(tileX+j == this->location->x and tileY+i == -this->location->y) continue;
				if(contains(bestPath,TBAGame->gameWorld->getTileAt(tileX+j,tileY+i))) continue;
				if(!TBAGame->gameWorld->getTileAt(tileX+j,tileY+i)->isPassable()) continue;
				// std::cout << "Testing tile " << tileX+j << ", " << tileY+i << std::endl;
				// debug(thisTile->x+j);
				tg = dist({this->location->x,-this->location->y},{tileX+j,tileY+i});
				th = dist({tileX+j,tileY+i},{tx,ty});
				f = tg+th;

				if(f < currentBestF) {
						currentBestTile = TBAGame->gameWorld->getTileAt(tileX+j,tileY+i);
						currentBestF = f;
						currentBestTile->highlight = true;
						previousBests.push_back(currentBestTile);
					
				}
			}
		}
		tileX = currentBestTile->x;
		tileY = -currentBestTile->y;
		if((char*)thisTile == (char*)currentBestTile) return true;
		if(iter > 80) {
			// debug("Pathing error");
			// this->targetPath = {};
			return false;
		}
		
		bestPath.push_back(currentBestTile);		
		iter++;
		
		if(adjacent) {
			if(currentBestTile->adjacent(targetTile)) break;
		}
		
	}
	
	if(!this->hasPath() or !TBAGame->gameWorld->validatePath(this->targetPath)) this->targetPath = bestPath;
	// this->targetPath = bestPath;
	
	this->lastPathCheck = TBAGame->logicTicks;
	
	for(int i=0;i<bestPath.size();i++) {
		
		// if(!bestPath.at(i)->hasBlocks()) bestPath.at(i)->addBlock(2);
		// std::cout << bestPath.at(i)->x << ", " << bestPath.at(i)->y << std::endl;
		
	}
	// debug(this->getName()+" is following new path ");
	// if(this->getName() != "Lost Bladesman") return true;
	for(int i=0;i<this->targetPath.size();i++) {
		// debug(std::to_string(this->targetPath.at(i)->x)+", "+std::to_string(this->targetPath.at(i)->y));
	}
	return true;
}{\rtf1}
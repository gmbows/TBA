void TextScreen::addContent(const std::string& s) {
	//Append a string to the content of the screen
	//And format it for display (no wrapping words)

	//Split new string into list of words but retain token character (space)
	std::vector<std::string> wordList = rsplit(' ',s);

	if(this->content.size() == 0) {
		this->content.push_back("");
	}

	int lineIndex = this->content.size()-1;
	std::string thisLine = this->content.at(lineIndex);
			std::string newWord;

	for(int i=0;i<wordList.size();i++) {

		lineIndex = this->content.size()-1;
		newWord = wordList.at(i);

		//Checking if adding the next word will overflow the width of the screen
		if(thisLine.size()+newWord.size() > this->screenCharWidth) {
			
			this->setLastLine(thisLine);
			thisLine = newWord;

			this->content.push_back("");
			lineIndex++;
		} else {
			thisLine += newWord;
		}
	}
	
	this->setLastLine(thisLine);

}

void TextScreen::setLastLine(const std::string& goodLine) {
	//Add a new line to the screen that contains 
	//string goodLine that we know doesn't overflow the screen
	//Handles special characters

	int lineIndex = this->content.size()-1;
	int midline = this->screenCharWidth/2;
	int cursorX;

	this->content.at(lineIndex) = "";

	for(int i=0;i<goodLine.size();i++) {
		if(goodLine[i] == '\n') {
			//If the line contains a newline character
			//Bump us down to the next line and don't add the newline character
			this->content.push_back("");
			lineIndex++;
		} else if (goodLine[i] == '\t') {
			cursorX = this->content.at(lineIndex).size();
			if(cursorX == 0) {
				//debug("START");
				this->content.at(lineIndex) += ' ';
			} else if(cursorX < midline) {
				//debug("BEFORE");
				while(cursorX < midline) {
					this->content.at(lineIndex) += ' ';
					cursorX = this->content.at(lineIndex).size();
				}
			} else if(cursorX > midline) {
				//debug("AFTER");
				this->content.push_back(" ");
				lineIndex++;
			}
		} else {
			if(goodLine[i] == '') {
				this->content.at(lineIndex) += ' ';
			} else {
				this->content.at(lineIndex) += goodLine[i];
			}
		}
	}

}

void MapScreen::drawMap() {

	//int dx = this->lastMapX-TBAGame->playerChar->x;
	//int dy = this->lastMapY-TBAGame->playerChar->y;

	int x = TBAGame->playerChar->x;
	int y = TBAGame->playerChar->y;

	this->map = TBAGame->gameWorld->getMapAt(this->screenCharWidth+2,x,y);
	this->lastMapX = x;
	this->lastMapY = y;
	//Source rectangle taken from screenFont->fontTexture
	SDL_Rect sRect;
	fChar charInfo;

	//Destination rectangle on screen
	SDL_Rect dRect;

	//Format raw content string for display
	//std::vector<std::vector<Tile*>> visibleContent = subVec(this->map,0,this->map.size());

	//visibleContent.push_back("->");
	Tile* thisTile;

	//Cursor
	int cursor[2] = {0,0};

	//Text offsets
	int offsetX = -this->charW;
	int offsetY = -this->charW;

	int tileID;
	int occupierTileID;
	int index = -1;


	//For each line of visible Content
	for(int i=0;i<map.size();i++) {
		for(int j=0;j<map.size();j++) {
			thisTile = this->map.at(i).at(j);

			//Get tile ID from tile and get tile texture from tileset

			tileID = thisTile->id;

			if(this->screenFont->fontMap.find(tileID) == this->screenFont->fontMap.end()) {
				debug("ERROR: Missing charMap entry for tile "+tileID);
				exit(0);
			}

			charInfo = this->screenFont->fontMap.at(tileID);

			//Read values from charInfo into SDL_Rect
			sRect = {charInfo.x,charInfo.y,charInfo.w,charInfo.h};

			//Modify display location rectangle based on cursor and font display values << FIX
			SDL_Rect dRect = {offsetX+this->x+(1*cursor[0]),offsetY+(charH*cursor[1])+this->y+charInfo.yo,sRect.w,sRect.h};

			//SDL_RenderCopy(TBAGame->gameWindow->renderer,this->screenFont->fontTexture,&sRect,&dRect);
			SDL_RenderCopyEx(TBAGame->gameWindow->renderer,this->screenFont->fontTexture,&sRect,&dRect,thisTile->rotation,NULL,thisTile->flip);

			if(thisTile->occupied) {
				occupierTileID = thisTile->occupier->displayID;

				charInfo = this->screenFont->fontMap.at(occupierTileID);

				//Read values from charInfo into SDL_Rect
				sRect = {charInfo.x,charInfo.y,charInfo.w,charInfo.h};

				//Modify display location rectangle based on cursor and font display values << FIX
				SDL_Rect dRect = {offsetX+this->x+(1*cursor[0]),offsetY+(charH*cursor[1])+this->y+charInfo.yo,sRect.w,sRect.h};

				//SDL_RenderCopy(TBAGame->gameWindow->renderer,this->screenFont->fontTexture,&sRect,&dRect);
				SDL_RenderCopyEx(TBAGame->gameWindow->renderer,this->screenFont->fontTexture,&sRect,&dRect,0,NULL,SDL_FLIP_NONE);
			}
			
			//Advance cursor for next character
			cursor[0] += charInfo.adv;
		}
		//Advance cursor to next line
		cursor[1]++;
		cursor[0] = 0;
	}
}
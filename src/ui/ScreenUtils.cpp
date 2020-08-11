#include "Screen.h"	

#include "../game/Input.h"
#include "../common/Common.h"
#include "../game/Projectile.h"
#include "../tools/Utility.h"

#include <unordered_set>

//========================
//CONTENT DRAWING FUNCTIONS
//========================

void TextScreen::addContent(const std::string& str) {
// debug("1");
	std::string s = str;
	// s=clean(s);
	
	int nullChars;
	
	if(this->content.size() == 0) {
		this->content.push_back("");
		if(s[0] == '\n') {
			s.erase(0,1);
		}
	}

	int indexOfLastSpace = -1;
	int tabStop = (this->screenCharWidth/2);
	int cursorX;

	bool colored = false;
	std::string thisLine;
	//Read last content line into a string and delete it from content vector
	if(this->content.at(this->content.size()-1)[this->content.at(this->content.size()-1).size()-1] == '\r') {
		// thisLine = this->content.at(this->content.size()-1);
		this->content.pop_back();
	}

	if(this->content.size() > 0) {
		thisLine = this->content.at(this->content.size()-1);
		this->content.pop_back();
	} else {
		return;
	}
	//Iterate through added string
	for(int i=0;i<s.size();i++) {
		cursorX = thisLine.size()-1;
		if(tsize(thisLine) + 1 >= this->screenCharWidth) {
			//Push content back with current line and start new line
			if(s[i] == ' ') {
				//If last character in line is a space, start new line without the space
				this->content.push_back(thisLine);
				thisLine = "";
				indexOfLastSpace = -1;
			} else if(indexOfLastSpace == -1) {
				//If the line has no spaces, break word
				this->content.push_back(thisLine);
				thisLine = s[i];
				indexOfLastSpace = -1;
			} else {
				//If the last character is not a space and there are spaces in the line
				//Break line on the last space
				thisLine += s[i];
				this->content.push_back(thisLine.substr(0,indexOfLastSpace));
				//thisLine = thisLine.substr(indexOfLastSpace+1,tsize(thisLine));
				s.insert(i+1,thisLine.substr(indexOfLastSpace,tsize(thisLine)));
				thisLine = "";
			}
		} else {
			// if((int)s[i] == -70 or (int)s[i] == -104) continue;
			// if((int)s[i] < 0) debug((int)s[i]);
			if(s[i] == ' ') {
				//Record index of the last space on this line
				thisLine += ' ';
				indexOfLastSpace = thisLine.size()-1;
			} else if(s[i] == '') {
				//Non-breaking space
				//Add space but don't record as a breakpoint
				thisLine += ' ';
			} else if(s[i] == '\n') {
				//Create new line and drop newline character
				this->content.push_back(thisLine);
				thisLine = "";
				indexOfLastSpace = -1;
			}	else if(s[i] == '\t') {
					//Move "cursor" to next tabstop
					if(cursorX  <= 1) {
						thisLine += ' ';
						indexOfLastSpace = thisLine.size()-1;
					} else if(cursorX < tabStop) {
						while(cursorX <= tabStop) {
							thisLine += ' ';
							cursorX = tsize(thisLine);
						}
						indexOfLastSpace = thisLine.size()-1;
					} else if(cursorX-nullchars(thisLine) >= tabStop) {
						this->content.push_back(thisLine);
						thisLine = " ";
						indexOfLastSpace = 0;
				}
			} else {
				//	if(s[i] == '') TBAGame->gameRunning = false;
				//Character is a traditional character, add to line
				thisLine += s[i];
			}

		}
	}
	this->content.push_back(thisLine);
	
}

std::vector<std::string> TextScreen::prepareCommandForDisplay(const std::string& s) {
	
	//Convert string command line to string vector formatted for the display
	//Very similar to addContent except special characters (newline, tab)
	//Don't need to be accounted for

	std::vector<std::string> preparedLines = {""};
	int lineIndex = 0;
	std::string thisLine;

	for(int i=0;i<s.size();i++) {
	
		if(thisLine.size()+1 >= this->screenCharWidth) {

			preparedLines.at(lineIndex) = thisLine;
			preparedLines.push_back("");
			lineIndex++;
			thisLine = s[i];

		} else {
			thisLine += s[i];
		}
	}

	preparedLines.at(lineIndex) = thisLine;
	return preparedLines;

}

void TextScreen::addContent(char c) {

	std::string thisLine = this->content.at(this->content.size()-1);

	if(thisLine.size() + 1 >= this->screenCharWidth) {
		this->content.push_back("");
	}
	int lineIndex = this->content.size()-1;
	this->content.at(lineIndex) += c;

}

void TextScreen::setLastLine(const std::string& goodLine) {
}

void MapScreen::generateMapTiles() {
	int centerX,centerY;
	if(TBAGame->playerChar == nullptr) {
		centerX = 0;
		centerY = 0;
	} else {
		centerX = TBAGame->playerChar->x;
		centerY = TBAGame->playerChar->y;
	}

	this->map = TBAGame->gameWorld->getMapAt((this->w/this->charW)+2,centerX,centerY);
	this->lastMapX = centerX;
	this->lastMapY = centerY;
}

void MapScreen::redrawActiveTiles() {

	SDL_SetRenderTarget(TBAGame->gameWindow->renderer,TBAGame->gameWorld->worldTexture);
	
	//Source rectangle taken from screenFont->fontTexture
	SDL_Rect sRect;
	fChar charInfo;

	//Destination rectangle on screen
	SDL_Rect dRect;

	Tile* thisTile;

	//Cursor
	int cursor[2] = {0,0};

	int tileID;
	int occupierTileID;
	int index = -1;

	int charSize;

	this->checked.clear();
	
	for(int i=0;i<map.size();i++) {
		for(int j=0;j<map.size();j++) {
			thisTile = this->map.at(i).at(j);

			//Criteria for activity:
			//	Occcupied by character or object


			//Update 3x3 area surrounding tiles occupied by characters
			if(thisTile->isOccupied() or thisTile->hasObjects() or thisTile->planted) {
				charSize = this->charW;
				int tcursor[2] = {cursor[0],cursor[1]};
				for(int k=-1;k<=1;k++) {
					for(int n=-1;n<=1;n++) {
						tcursor[0] = cursor[0]+(n*charW);
						tcursor[1] = cursor[1]+k;
						try {
							thisTile = this->map.at(std::min(i+k,(int)this->map.size()-1)).at(std::min(j+n,(int)this->map.size()-1));
						} catch(const std::exception &e) {
							continue;
						}
						//Check if this tile has been updated already
						if(this->checked.find((char*)thisTile) != this->checked.end()) {
							continue;
						} else {
							this->checked.insert((char*)thisTile);
						}

						tileID = thisTile->getDisplayID();

						if(this->screenFont->fontMap.find(tileID) == this->screenFont->fontMap.end()) {
							debug("ERROR: Missing charMap entry for tile "+tileID);
							exit(0);
						}

						charInfo = this->screenFont->fontMap.at(tileID);

						int windowOffsetX = ((TBAGame->gameWorld->size/2)+thisTile->x)*this->charW;
						int windowOffsetY = ((TBAGame->gameWorld->size/2)-thisTile->y)*this->charH;

						sRect = {charInfo.x,charInfo.y,charInfo.w,charInfo.h};
						dRect = {windowOffsetX,windowOffsetY,charSize,charSize};

						// if(thisTile->planted) SDL_SetTextureAlphaMod(this->screenFont->fontTexture,120);
						SDL_RenderCopyEx(TBAGame->gameWindow->renderer,this->screenFont->fontTexture,&sRect,&dRect,thisTile->getRotation(),NULL,thisTile->getFlip());
						// SDL_SetTextureAlphaMod(this->screenFont->fontTexture,255);

					}
				}
			}

			

			//Advance cursor for next character
			cursor[0] += this->charW;
		}
		//Advance cursor to next line
		cursor[1]++;
		cursor[0] = 0;

	}
}

void MapScreen::updateMap() {

	int centerX,centerY;
	if(TBAGame->playerChar == nullptr) {
		centerX = 0;
		centerY = 0;
	} else {
		centerX = TBAGame->playerChar->x;
		centerY = TBAGame->playerChar->y;
	}

	if(centerX != this->lastMapX or centerY != this->lastMapY) {
		this->generateMapTiles();
	}

	SDL_SetRenderTarget(TBAGame->gameWindow->renderer,TBAGame->gameWorld->worldTexture);

	// /SDL_RenderClear(TBAGame->gameWindow->renderer);

	//Source rectangle taken from screenFont->fontTexture
	SDL_Rect sRect;
	fChar charInfo;

	//Destination rectangle on screen
	SDL_Rect dRect;

	Tile* thisTile;

	//Cursor
	int cursor[2] = {0,0};

	int tileID;
	int occupierTileID;
	int index = -1;

	int charSize;

	SDL_Rect charRect;

	Character* occupant;
	GameObject* generic;

	float objX,objY;

	int objOffsetX;
	int objOffsetY;

	//make initial pass to update active tiles
	this->redrawActiveTiles();
	
	for(int i=0;i<map.size();i++) {
		for(int j=0;j<map.size();j++) {

				thisTile = this->map.at(i).at(j);
				
				charSize = this->charW;
				
				
				tileID = thisTile->getDisplayID();
				

				if(this->screenFont->fontMap.find(tileID) == this->screenFont->fontMap.end()) {
					debug("ERROR: Missing charMap entry for tile "+tileID);
					exit(0);
				}

				charInfo = this->screenFont->fontMap.at(tileID);



				int windowOffsetX = ((TBAGame->gameWorld->size/2)+thisTile->x)*this->charW;
				int windowOffsetY = ((TBAGame->gameWorld->size/2)-thisTile->y)*this->charH;

				sRect = {charInfo.x,charInfo.y,charInfo.w,charInfo.h};
				dRect = {windowOffsetX,windowOffsetY,charSize,charSize};

				if(thisTile->highlight) {
					SDL_SetTextureColorMod(this->screenFont->fontTexture,255,255,255);
				}
				
				
				if(thisTile->hasBlocks()) {

					SDL_RenderCopyEx(TBAGame->gameWindow->renderer,thisTile->getBlockTexture(),NULL,&dRect,thisTile->getRotation(),NULL,thisTile->getFlip());

				} else if(thisTile->needsUpdate or thisTile->highlight) {
					SDL_RenderCopyEx(TBAGame->gameWindow->renderer,this->screenFont->fontTexture,&sRect,&dRect,thisTile->getRotation(),NULL,thisTile->getFlip());
					thisTile->needsUpdate = false;
				}
				
				
				if(thisTile->planted) {
					tileID = 9;
					charInfo = this->screenFont->fontMap.at(tileID);
					sRect = {charInfo.x,charInfo.y,charInfo.w,charInfo.h};
					dRect = {windowOffsetX,windowOffsetY,charSize,this->charW};
					SDL_SetTextureAlphaMod(this->screenFont->fontTexture,120);
					SDL_RenderCopy(TBAGame->gameWindow->renderer,this->screenFont->fontTexture,&sRect,&dRect);
					SDL_SetTextureAlphaMod(this->screenFont->fontTexture,255);
				}

				for(int k=0;k<thisTile->occupiers.size();k++) {

					charSize = 5;

					occupant = thisTile->occupiers.at(k);

					charInfo = TBAGame->gameWindow->textScreen->screenFont->fontMap.at((int)occupant->getName()[0]);

					int windowOffsetX = 5+((TBAGame->gameWorld->size/2)+occupant->x)*this->charW;
					int windowOffsetY = 5+((TBAGame->gameWorld->size/2)+occupant->y)*this->charH;

					sRect = {charInfo.x,charInfo.y,charInfo.w,charInfo.h};
					dRect = {windowOffsetX,windowOffsetY,charSize,charSize};

					if(!occupant->isAlive()) {
						SDL_SetTextureColorMod(TBAGame->gameWindow->textScreen->screenFont->fontTexture,140,140,140);
					}

					SDL_RenderCopy(TBAGame->gameWindow->renderer,TBAGame->gameWindow->textScreen->screenFont->fontTexture,&sRect,&dRect);
					//Draw viewAng indicator
					
					if(!occupant->isAlive()) {
						SDL_SetTextureColorMod(TBAGame->gameWindow->textScreen->screenFont->fontTexture,200,200,200);
					} else {
						SDL_RenderDrawLine(TBAGame->gameWindow->renderer,windowOffsetX+2.5+std::cos(occupant->viewAng*CONV_DEGREES)*5,windowOffsetY+2.5+std::sin(occupant->viewAng*CONV_DEGREES)*5,windowOffsetX+2.5+std::cos(occupant->viewAng*CONV_DEGREES)*10,windowOffsetY+2.5+std::sin(occupant->viewAng*CONV_DEGREES)*10);
					}
}

				for(int k=0;k<thisTile->objects.size();k++) {
					generic = thisTile->objects.at(k);

					//Placeholder for arrow texture
					charInfo = this->screenFont->fontMap.at(generic->getDisplayID());
					
					decompose(generic->getLocation(),objX,objY);

					int windowOffsetX = ((TBAGame->gameWorld->size/2)+objX)*this->charW;
					int windowOffsetY = ((TBAGame->gameWorld->size/2)+objY)*this->charH;

					sRect = {charInfo.x,charInfo.y,charInfo.w,charInfo.h};

					SDL_SetTextureColorMod(this->screenFont->fontTexture,255,255,255);

					switch(generic->type) {
						case OBJ_PROJECTILE:
							charSize = static_cast<Projectile*>(generic)->displaySize;
							windowOffsetX = ((TBAGame->gameWorld->size/2)+objX+.22)*this->charW;
							windowOffsetY = ((TBAGame->gameWorld->size/2)+objY+.22)*this->charH;
							dRect = {windowOffsetX,windowOffsetY,charSize,charSize};
							SDL_RenderCopyEx(TBAGame->gameWindow->renderer,this->screenFont->fontTexture,&sRect,&dRect,static_cast<Projectile*>(generic)->angle*CONV_RADIANS,NULL,SDL_FLIP_NONE);
							break;
						case OBJ_INTERACTIVE:
							charSize = this->charW;
							dRect = {windowOffsetX,windowOffsetY,charSize,charSize};
							SDL_RenderCopy(TBAGame->gameWindow->renderer,this->screenFont->fontTexture,&sRect,&dRect);
							break;
						case OBJ_CONTAINER:
							charSize = this->charW;
							dRect = {windowOffsetX,windowOffsetY,charSize,charSize};
							SDL_RenderCopy(TBAGame->gameWindow->renderer,this->screenFont->fontTexture,&sRect,&dRect);
							break;
					}
				}
				
				SDL_SetTextureColorMod(this->screenFont->fontTexture,200,200,200);

			//Advance cursor for next character
			cursor[0] += this->charW;
		}
		//Advance cursor to next line
		cursor[1]++;
		cursor[0] = 0;

	}

	SDL_SetRenderTarget(TBAGame->gameWindow->renderer,NULL);
}

void Screen::shiftContentWindow(int i) {

	this->contentWindowOffset -= 2*i;
}

void TextScreen::resetContentWindowOffset() {
	this->contentWindowOffset = 0;
}

void TextBox::setContent(const std::string& content) {
	this->content.clear();
	this->addContent(content);
}

void TextBox::prepareContent() {
	std::string newContent = this->getContent();
	this->setContent(newContent);
	this->oldContent = newContent;
}

std::string TextBox::getContent() {
	std::string newContent; 
		// TBAGame->playerChar->getName()+":"+TBAGame->playerChar->inventory->toString();

		//Draw display target info
		/*if(TBAGame->playerChar->hasTarget()) {
			newContent += TBAGame->playerChar->getTargetInfo();

		}*/

		if(TBAGame->hasDisplayTarget()) {
			if(TBAGame->displayTarget->type == OBJ_CHARACTER) {
				newContent += TBAGame->displayTarget->getAsCharacter()->getInfo();
				newContent += "\nInventory:"+TBAGame->displayTarget->getAsCharacter()->getInvString();
				newContent += "\n"+TBAGame->displayTarget->getAsCharacter()->getEquipString();
			} else {
				newContent += TBAGame->displayTarget->getInfo();
			}
        }
	return newContent;
}

void DynamicTextBox::setContent(const std::string& content) {
	this->content.clear();

	//Reset box dimensions based on new content
	this->w = std::min(this->maxWidth*charW,(this->charW*(int)content.size()) + (this->borderSize*2));
	this->screenCharWidth = this->w/this->charW;

	this->h = (this->charH*((int)content.size()/this->screenCharWidth+1)) + 6;
	this->screenCharHeight = this->h/this->charH;

	this->x = this->defaultX - this->w/2;
	//this->y = this->defaultY + this->h/2;

	this->addContent(content);
}

//=================
//COMMAND FUNCTIONS
//=================

void TextScreen::setCommand(const std::string& s) {

	//Set the potentially incomplete command at the end of command history
	this->command = s;
	this->commandHistory.at(this->commandHistory.size()-1) = s;
	this->setCommandLine();
	this->resetContentWindowOffset();

}

void TextScreen::setCommandLine() {
	this->commandLines = this->prepareCommandForDisplay(tabChar+command);
}

void TextScreen::commandAppend(char c) {
	this->setCommand(this->command+c);
}

void TextScreen::commandAppend(const std::string& s) {
	this->setCommand(this->command+s);
}

void TextScreen::deleteLastCharacter() {

	if(this->command.size() > 0) {
		this->setCommand(this->command.substr(0,this->command.size()-1));
	}

}

void TextScreen::sendCommand() {

	std::string commandReturn;

	//COMMAND HISTORY HANDLING
	//If command history has more than 1 entry
	//If this command is the same as the last one, don't add it to command history
	if(this->commandHistory.size() > 1) {
		if(this->command != this->commandHistory.at(this->commandHistory.size()-2)) {
			this->commandHistory.push_back("");
			this->commandIndex++;
		}
	} else {
		this->commandHistory.push_back("");
		this->commandIndex++;
	}

	//Add command line to history
	this->content = extend(this->content,this->commandLines);

	//Newlines are added automatically before and after command line
	//Retrieve command return from input function
	commandReturn = processCommand(this->command);

	//Add command to content
	//Command line is reset and added automatically whenever the command is updated
	if(commandReturn.size() > 0) {
		this->addContent(commandReturn);
	}
	this->setCommand("");
	this->commandIndex = this->commandHistory.size()-1;

}

void TextScreen::swapCommand(int i) {
	//Swaps commands from commandHistory
	//For use with command history functionality

	this->commandIndex += i;

	if(this->commandHistory.size() == 0) {return;}

	if(this->commandIndex < 0) {
		this->commandIndex = 0;
	} else if(this->commandIndex > this->commandHistory.size()-1) {
		this->commandIndex = this->commandHistory.size()-1;
	}

	this->setCommand(this->commandHistory.at(commandIndex));
	//this->command = this->commandHistory.at(commandIndex);

}

void TextScreen::addCommand() {

	//Add command to command history unless duplicate of previous command
	if(this->command != this->commandHistory.at(this->commandHistory.size()-1)) {
		this->commandHistory.push_back(this->command);
		commandIndex++;
	}

}

//=================
//    MISC FUNCTIONS
//=================

bool Screen::enclose(int x,int y) {
	//Determine if point x,y is within this screen

	if(x >= this->x and x < this->x+this->w) {
		if(y >= this->y and y < this->y+this->h) {
			return true;
		}
	}
	return false;
}
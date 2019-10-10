#include "Game.h"

#include <thread>

#include "../ui/Window.h"
#include "../ui/Screen.h"

#include "../tools/StringFuncs.h"

#include "../game/GameObject.h"
#include "../game/World.h"
#include "../game/Character.h"
#include "../game/Item.h"
#include "../game/Container.h"
#include "../game/Inventory.h"
#include "../game/Command.h"
#include "../game/CommandFuncs.h"
#include "../game/FloatingText.h"
#include "../game/Structure.h"

//=============
//	 	SETUP
//=============

void Game::setupUI() {

	this->gameRunning = true;
	this->gameLog = new Log("Game.txt");

	gameLog->writeln("Beginning game startup\n");

	int textW = 8;
	int textH = 12;

	int tileW = 16;

	int borderSize = 25;
	int tScreenW = textW*40+8;
	int tScreenH = textH*40+6;

	//+1 because border draws one pixel into the screen area
	int quadSize = tileW-1;			
	int mapScreenS = (tileW*((quadSize*2)+1))+1;

	tScreenH = mapScreenS+borderSize;

	gameLog->writeln("Creating window and screen objects...");
	TextScreen *textScreen = new TextScreen(borderSize,borderSize,tScreenW,tScreenH,true);
	MapScreen *mapScreen = new MapScreen((borderSize*2)+tScreenW,borderSize,mapScreenS,mapScreenS,true);
	TextBox *auxScreen = new TextBox((borderSize*3)+tScreenW+mapScreenS,borderSize,tScreenW,tScreenH,true);
	DynamicTextBox *popupBox = new DynamicTextBox("Cannot place building here",mapScreen->x+(mapScreenS/2),borderSize+(borderSize/2)+mapScreenS);
	gameLog->writeln("Screen objects initialized");

	this->gameWindow = new Window((borderSize*4)+tScreenW*2+mapScreenS,600);
	gameLog->writeln("Window object initialized");

	//std::string a = "It was to have its service of the intellect, certainly; yet, it was never to accept any theory or system that would involve the sacrifice of any mode of passionate experience. Its aim, indeed, was to be experience itself, and not the fruits of experience, sweet or bitter as they might be. Of the asceticism that deadens the senses, of the vulgar profligacy that dulls them, it was to know nothing. But it was to teach man to concentrate himself upon the moments of a life that is itself but a moment.";
	//std::string b = "There ar\n\n\n\n\n\n\n\n\ne few of us who have not sometimes wakened before dawn, either after one of those dreamless nights that make us almost enamored of death, or one of those nights of horror and misshapen joy, when through the chambers of the brain sweep phantoms more terrible than reality itself, and instinct with that vivid life that lurks in all grotesques, and that lends to Gothic art its enduring vitality, this art being, one might fancy, especially the art of those whose minds have been troubled with the malady of reverie. Gradually white fingers creep through the curtains, crawl into the corners of the room, and crouch there. Outside, there is the stirring of birds among the leaves, or the sound of men going forth to their work, or the sigh and sob of the wind coming down from the hill, and wandering round the silent house, as though it feared to wake the sleepers, and yet must needs call forth sleep from her purple cave. Veil after veil of thin dusky gauze is lifted, and by degrees the forms and colors of things are restored to them, and we watch the dawn remaking the world in its antique pattern. The wan mirrors get back their mimic life. The flameless tapers stand where we had them, and beside them lies the half-cut book that we had been studying, or the wired flower that we had worn at the ball, or the letter that we had been afraid to read, or that we had read too often. Nothing seems to us changed. Out of the unreal shadows of the night comes back the real life that we had known. We have to resume it where we had left off, and there steals over us a terrible sense of the necessity for the continuance of energy in the same wearisome round of stereotyped habits, or a wild longing, it may be, that our eyelids might open some morning upon a world in which things would have fresh shapes and colors, and be changed, or have other secrets, a world in which the past would have little or no place, or survive, at any rate, in no conscious form of obligation or regret,\n\n\n\n\n\n\n\n\n\n the remembrance even of joy having its bitterness, and the memories of pleasure their pain.";

	//textScreen->addContent("Type ? for a list of commands");

	//Core screen objects
	this->gameWindow->textScreen = textScreen;
	this->gameWindow->mapScreen = mapScreen;
	this->gameWindow->auxScreen = auxScreen;
	this->gameWindow->popupBox = popupBox;
	this->gameWindow->mapPanel = new Panel();

	mapScreen->mapTexture = SDL_CreateTexture(this->gameWindow->renderer,
                               SDL_GetWindowPixelFormat( this->gameWindow->window),
								SDL_TEXTUREACCESS_TARGET,
                               mapScreen->w+(2*mapScreen->charW),
                               mapScreen->h+(2*mapScreen->charH));

	//Map panel to obscure mapscreen overlap
	SDL_Rect fillTop = {0,0,this->gameWindow->width,borderSize-1};
	SDL_Rect fillLeft = {textScreen->x+textScreen->w-6,0,borderSize+5,gameWindow->height};
	SDL_Rect fillRight = {mapScreen->x+mapScreenS+2,0,borderSize+3,gameWindow->height};
	SDL_Rect fillBottom = {0,borderSize+mapScreenS+2,this->gameWindow->width,mapScreenS};

	//Draw 4 panels around map screen to block map overlap
	this->gameWindow->mapPanel->panelVector = {fillTop,fillLeft,fillRight,fillBottom};

	//Add core movable screens to screenVector
	this->gameWindow->screenVector = {textScreen,auxScreen};

	gameLog->writeln("Window object fully initialized, generating font textures");

	this->gameWindow->generateScreenFontTextures();

	gameLog->writeln("Font textures generated, game startup complete\n");

}

void Game::setupGame() {
	
	//this->maxWaitTime = 1000/std::max(this->logicTickRate,this->graphicsTickRate);
	//this->minWaitTime = 1000/std::min(this->logicTickRate,this->graphicsTickRate);
						
	this->commandList = {
			//							name,alias,argc,commandFunc,ECfunc=nullptr
			new		Command("help",{"?"},0,helpFunc,helpEC),
			new 		Command("clear",{"clr"},0,clearFunc),
			new 		Command("inventory",{},0,inventoryFunc),
			new		Command("move",{"mv"},1,moveFunc,moveEC),
			new		Command("pause",{},0,pauseFunc),
			new		Command("stop",{},0,stopFunc),
			new		Command("unpause",{},0,unpauseFunc),
			new		Command("target",{},1,targetFunc,targetEC),
			new		Command("attack",{"atk"},1,attackFunc,attackEC),
			new		Command("zoom",{"zm"},1,zoomFunc,zoomEC),
			new		Command("say",{},1,sayFunc,sayEC),
			new		Command("take",{},1,takeFunc,takeEC),
			new		Command("hurtme",{"hurt"},1,hurtmeFunc,hurtmeEC),
			new		Command("exit",{"exit","quit"},0,exitFunc),
			////
		};
	
	//Populate string command list with command names for autocomplete
	for(int i=0;i<this->commandList.size();i++) {
		this->commandStrings.push_back(this->commandList.at(i)->name);
	}

	//Length of one edge of map square
	int quadSize = 128;

	this->gameWorld = new World(quadSize*2);
	this->gameWorld->genWorld();

	//Create player and fill inventory with generic items
	new Character(true,160,"Player",0,0);
	this->playerChar->inventory->add({0,1,0,1,0,1,0,1,0,1,0,1,0,1,3,3,3,3,3,3,3,3,3,3,0,1,0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,2,2,2});

	//New characters are added to gameObjects automatically
	Character* newChar;
	for(int i=0;i<1;i++) {
		newChar = new Character(false,160,"Looter "+std::to_string(i+1),(rand()%(1+(quadSize*2)))-quadSize,(rand()%(1+(quadSize*2)))-quadSize);
		newChar->equipment->primary = new Item(4);
		newChar->setTarget(this->playerChar);
		newChar->setStatus(COMBAT);
		//newChar->setTarget(this->playerChar);
		//new Character(false,160,"Looter",-quadSize+i+1,-quadSize+1+(i/quadSize));
	}
	newChar = new Character(false,160,"Debug Looter",1,0);
	newChar->setTarget(new Character(false,160,"Lost Bladesman",0,3));
	//newChar->equipment->primary = new Item(4);
	//newChar->setTarget(playerChar);
	newChar->setStatus(COMBAT);
	//static_cast<Character*>(this->gameObjects.at(2))->setTarget(newChar);
	//static_cast<Character*>(this->gameObjects.at(2))->setStatus(COMBAT);

	this->gameWorld->createStructure({0,0}, house, 4);
	new Container("Footlocker",{-1.0f,-1.0f},160,{3,3,3,3,3,3,3,3,4,3,0,1,2,1,2,1,2,1,2,1});
}

//=======================
//			GAME OBJECTS
//=======================

GameObject* Game::findObject(int id) {
		for(int i=0;i<this->gameObjects.size();i++) {
		if(this->gameObjects.at(i)->objectID == id) {
			return this->gameObjects.at(i);
		}
	}
	return nullptr;
}

GameObject* Game::findObject(const std::string& name) {
	for(int i=0;i<this->gameObjects.size();i++) {
		if(this->gameObjects.at(i)->getName() == name) {
			return this->gameObjects.at(i);
		}
	}
	return nullptr;
}

void Game::removeObject(GameObject* o) {
	int objectIndex = find(o,this->gameObjects);
	if(objectIndex >= 0) {
		this->gameObjects.erase(this->gameObjects.begin()+objectIndex);
		this->gameObjects.shrink_to_fit();
	}
}

void Game::removeUIObject(GameObject* o) {
	int objectIndex = find(o,this->gameUIObjects);
	if(objectIndex >= 0) {
		this->gameUIObjects.erase(this->gameUIObjects.begin()+objectIndex);
		this->gameUIObjects.shrink_to_fit();
	}
}

//=============
//		MISC
//=============

bool Game::togglePause() {
	this->paused = !this->paused;
	if(this->paused) {
		this->gameWindow->popupBox->setToggledContent("Paused");
		this->gameWindow->popupBox->toggled = true;
	} else {
		this->gameWindow->popupBox->toggled = false;
	}
	return this->paused;
}

void Game::popupText(int duration, const std::string& message) {

	//Duration in seconds
	this->gameWindow->popupBox->addMessage(duration*1000,message);

}

//=============
//		UPDATE
//=============

void Game::updateGameObjects() {
	for(int i=0;i<this->gameObjects.size();i++) {
		this->gameObjects.at(i)->update();
	}

}

void Game::updateGameUIObjects() {
	for(int i=0;i<this->gameUIObjects.size();i++) {
		this->gameUIObjects.at(i)->update();
	}
}

void Game::update() {

	if(SDL_GetTicks() >= this->lastGraphicsUpdate + (1000/this->graphicsTickRate)) {
		//Update game window and all screens
		this->lastGraphicsUpdate = SDL_GetTicks();
		this->gameWindow->update();
		this->graphicsTicks++;
		this->timeToNextGraphicsUpdate = (this->lastGraphicsUpdate + (1000/this->graphicsTickRate)) - SDL_GetTicks();
	}
	//Suspend logic ticks if game is paused
	if(!this->paused) {
		if(SDL_GetTicks() >= this->lastLogicUpdate + (1000/this->logicTickRate)) {
			//Update all active game objects
			this->lastLogicUpdate = SDL_GetTicks();
			this->updateGameObjects();
			this->logicTicks++;
			this->timeToNextLogicUpdate = (this->lastLogicUpdate + (1000/this->logicTickRate)) - SDL_GetTicks();
		}
	}

	//this->timeToNextGraphicsUpdate = (this->lastGraphicsUpdate + (1000/this->graphicsTickRate)) - SDL_GetTicks();
	//this->timeToNextLogicUpdate = (this->lastLogicUpdate + (1000/this->logicTickRate)) - SDL_GetTicks();

	this->timeToNextUpdate = std::min(this->timeToNextGraphicsUpdate,this->timeToNextLogicUpdate);

	if(this->timeToNextUpdate < 0) {
		if(!this->paused) {
			debug("Falling behind!");
		}
	}

	//std::cout << (std::to_string(this->timeToNextUpdate)+"\r");

	SDL_Delay(std::max(0,this->timeToNextUpdate));

}

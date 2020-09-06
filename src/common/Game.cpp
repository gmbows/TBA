#include "Game.h"

#include <thread>
#include <random>
#include <memory>

#include "../ui/Window.h"
#include "../ui/Screen.h"

#include "../tools/Utility.h"
#include "../tools/Algorithm.h"

#include "../game/GameObject.h"
#include "../game/World.h"
#include "../game/Character.h"
#include "../game/Item.h"
#include "../game/Container.h"
#include "../game/ResourceNode.h"
#include "../game/Inventory.h"
#include "../game/Command.h"
#include "../game/CommandFuncs.h"
#include "../game/FloatingText.h"
#include "../game/Structure.h"
#include "../game/Squad.h"
#include "../common/Keys.h"

#include "../game/Projectile.h"

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
	 // DynamicTextBox *popupBox = new DynamicTextBox("Cannot place building here",mapScreen->x+(mapScreenS/2),borderSize+(borderSize/2)+mapScreenS);
	gameLog->writeln("Screen objects initialized");

	this->gameWindow = new Window((borderSize*4)+tScreenW*2+mapScreenS,600);
	// Window* debugWindow = new Window(400,200);
	gameLog->writeln("Window object initialized");

	//std::string a = "It was to have its service of the intellect, certainly; yet, it was never to accept any theory or system that would involve the sacrifice of any mode of passionate experience. Its aim, indeed, was to be experience itself, and not the fruits of experience, sweet or bitter as they might be. Of the asceticism that deadens the senses, of the vulgar profligacy that dulls them, it was to know nothing. But it was to teach man to concentrate himself upon the moments of a life that is itself but a moment.";
	//std::string b = "There ar\n\n\n\n\n\n\n\n\ne few of us who have not sometimes wakened before dawn, either after one of those dreamless nights that make us almost enamored of death, or one of those nights of horror and misshapen joy, when through the chambers of the brain sweep phantoms more terrible than reality itself, and instinct with that vivid life that lurks in all grotesques, and that lends to Gothic art its enduring vitality, this art being, one might fancy, especially the art of those whose minds have been troubled with the malady of reverie. Gradually white fingers creep through the curtains, crawl into the corners of the room, and crouch there. Outside, there is the stirring of birds among the leaves, or the sound of men going forth to their work, or the sigh and sob of the wind coming down from the hill, and wandering round the silent house, as though it feared to wake the sleepers, and yet must needs call forth sleep from her purple cave. Veil after veil of thin dusky gauze is lifted, and by degrees the forms and colors of things are restored to them, and we watch the dawn remaking the world in its antique pattern. The wan mirrors get back their mimic life. The flameless tapers stand where we had them, and beside them lies the half-cut book that we had been studying, or the wired flower that we had worn at the ball, or the letter that we had been afraid to read, or that we had read too often. Nothing seems to us changed. Out of the unreal shadows of the night comes back the real life that we had known. We have to resume it where we had left off, and there steals over us a terrible sense of the necessity for the continuance of energy in the same wearisome round of stereotyped habits, or a wild longing, it may be, that our eyelids might open some morning upon a world in which things would have fresh shapes and colors, and be changed, or have other secrets, a world in which the past would have little or no place, or survive, at any rate, in no conscious form of obligation or regret,\n\n\n\n\n\n\n\n\n\n the remembrance even of joy having its bitterness, and the memories of pleasure their pain.";

	//textScreen->addContent("Type ? for a list of commands");

	//Core screen objects
	this->gameWindow->textScreen = textScreen;
	this->gameWindow->mapScreen = mapScreen;
	this->gameWindow->auxScreen = auxScreen;
	this->gameWindow->mapPanel = new Panel();

	mapScreen->mapTexture = SDL_CreateTexture(this->gameWindow->renderer,
                               SDL_GetWindowPixelFormat( this->gameWindow->window),
								SDL_TEXTUREACCESS_TARGET,
                               mapScreen->w+(2*mapScreen->charW),
                               mapScreen->h+(2*mapScreen->charH));

	textScreen->init_texture();
	auxScreen->init_texture();

	//Map panel to obscure mapscreen overlap
	SDL_Rect fillTop = {0,0,this->gameWindow->width,borderSize-1};
	SDL_Rect fillLeft = {textScreen->x+textScreen->w-6,0,borderSize+5,gameWindow->height};
	SDL_Rect fillRight = {mapScreen->x+mapScreenS+2,0,borderSize-3,gameWindow->height};
	SDL_Rect fillBottom = {0,borderSize+mapScreenS+2,mapScreenS+400,mapScreenS};

	//Draw 4 panels around map screen to block map overlap
	this->gameWindow->mapPanel->panelVector = {fillTop,fillLeft,fillRight,fillBottom};

	//Add core movable screens to screenVector
	this->gameWindow->screenVector = {textScreen,auxScreen};

	gameLog->writeln("Window object fully initialized, generating font textures");

	this->gameWindow->generateScreenFontTextures();

	gameLog->writeln("Font textures generated, game startup complete\n");

}

void Game::setupGame() {

	//Verify that we have help entries for each command
	checkHelp();
	
	//Import items from item file and make sure we have strings for each type
	importItems();
	checkItemTypes();

	this->gameLog = new Log("Game.txt");
	gameLog->writeln("Beginning game startup\n");
	
	pthread_mutex_init(&this->graphicsLock, NULL);
	
	//this->maxWaitTime = 1000/std::max(this->logicTickRate,this->graphicsTickRate);
	//this->minWaitTime = 1000/std::min(this->logicTickRate,this->graphicsTickRate);
						
	this->commandList = {
			//				first alias is primary command name
			//				name,alias,argc,commandFunc,ECfunc=nullptr
			//COMMANDSTART
			new		Command({"help","?"},helpFunc,helpEC),
			new 		Command({"clear","clr"},clearFunc),
			new 		Command({"inventory"},inventoryFunc),
			new		Command({"move","mv"},moveFunc,moveEC),
			new		Command({"pause"},pauseFunc),
			new		Command({"stop"},stopFunc),
			new		Command({"unpause"},unpauseFunc),
			new		Command({"target"},targetFunc,targetEC),
			new		Command({"attack","atk"},attackFunc,attackEC),
			new		Command({"zoom","zm"},zoomFunc,zoomEC),
			new		Command({"say"},sayFunc,sayEC),
			new		Command({"hurtme"},hurtmeFunc,hurtmeEC),
			new		Command({"exit","quit"},exitFunc),
			new		Command({"take"},takeFunc,takeEC),
			new		Command({"select","sel"},selectFunc,selectEC),
			new		Command({"put","give"},putFunc,putEC),
			new		Command({"search"},searchFunc,searchEC),
			new		Command({"equip"},equipFunc,equipEC),
			new		Command({"debug"},debugFunc),
			new		Command({"examine"},examineFunc,examineEC),
			new		Command({"plant"},plantFunc,plantEC),
			new		Command({"drink"},drinkFunc,drinkEC),
			new		Command({"use"},useFunc,useEC),
			new		Command({"giveme"},givemeFunc,givemeEC),
			new		Command({"work"},workFunc,workEC),
			new		Command({"goto"},gotoFunc,gotoEC),
			new		Command({"control"},controlFunc,controlEC),
			new		Command({"unequip"},unequipFunc,unequipEC),
			////
		};
	
	//Populate string command list with command names for autocomplete
	for(int i=0;i<this->commandList.size();i++) {
		this->commandStrings.push_back(this->commandList.at(i)->aliases.at(0));
	}

	//Length of one edge of map square
	int quadSize = 128;

	this->gameWorld = new World(quadSize*2);
	
	gameWorld->worldTexture = SDL_CreateTexture(this->gameWindow->renderer,
								SDL_GetWindowPixelFormat( this->gameWindow->window),
								SDL_TEXTUREACCESS_TARGET,
								gameWorld->size*this->gameWindow->mapScreen->charW,
								gameWorld->size*this->gameWindow->mapScreen->charH);
							   
	gameWorld->screenFont->generateFontTexture(this->gameWindow->window,this->gameWindow->renderer);
	this->gameWorld->genWorld();
	this->gameWorld->genWorld_new(this->gameWindow->renderer);

	//Create player and fill inventory with generic items
	this->playerChar = new Character("Player",160,{0,0});
	this->playerChar->giveItem(2);
	this->playerChar->viewAng = 180;
	for(int i=0;i<10;i++) {
		//Don't add null item
		this->playerChar->giveItem(1+(rand()%(itemManifest.size()-1)));
	}
	this->playerChar->giveItem(13);
	this->playerChar->giveItem(7);
	this->playerChar->giveItem(7);
	this->playerChar->giveItem(7);
	this->playerChar->giveItem(7);
	this->displayTarget = this->playerChar;
	
	//New characters are added to gameObjects automatically
	// Character *newChar,*LB,*Dog,*Chog;
	for(int i=0;i<0;i++) {
		// newChar = new Character("Looter "+std::to_string(i+1),(rand()%(1+(quadSize*2)))-quadSize,(rand()%(1+(quadSize*2)))-quadSize);
		// if(rand()%2 == 0) newChar->equipment->equip(new Item(4),EQUIP_PRIMARY);
		// newChar->setTarget(this->playerChar);
		// newChar->setStatus(STATUS_COMBAT);
		//newChar->setTarget(this->playerChar);
		//new Character(false,160,"Looter",-quadSize+i+1,-quadSize+1+(i/quadSize));
	}

	// newChar = new Character("Debug Trader",160,{0,-7});
	// Chog = new Character("Chog",160,{4,-2});

	// TBAGame->setDisplayTarget(newChar);
	// newChar->moveTo(0,0);
	// newChar->maxMoveSpeed = this->playerChar->maxMoveSpeed*2;
	// LB = new Character("Lost Bladesman",160,{-2,-1});
	// new Character("Worker",160,{-2,2});
	// new Character("Ondei",160,{0,6});
	// LB->giveItems({14,15,16,17});
	// LB->body->getLimb(LIMB_TORSO)->health = 100000;
	// LB->setTarget(this->playerChar);
	// LB->setStatus(STATUS_COMBAT);
	// Dog = new Character("Wolf",160,{5,5});
	// Dog->equipment->equip(new Item(13),EQUIP_PRIMARY);
	// Dog->giveItem(8);
	// Dog->giveItem(8);
	// Dog->giveItem(8);
	// Dog->giveItem(8);

	// LB->equipment->primary = new Item(4);
	// Dog->maxMoveSpeed = playerChar->maxMoveSpeed*2;
	// Dog->turnSpeed = playerChar->turnSpeed*2;
	// newChar->lookAt(LB);
	//newChar->setTarget(LB);
	// Dog->setTarget(LB);
	// Dog->setStatus(STATUS_COMBAT);
	//newChar->equipment->primary = new Item(4);
	//newChar->setTarget(playerChar);
	//newChar->setStatus(STATUS_COMBAT);
	//static_cast<Character*>(this->gameObjects.at(2))->setTarget(newChar);
	//static_cast<Character*>(this->gameObjects.at(2))->setStatus(STATUS_COMBAT);	
	init_structures();
	this->gameWorld->createStructure({0,0}, random, 4);
	new Container("Footlocker",{-1.0f,-1.0f},160,{3,3,3,3,3,3,3,3,4,3,1,1,2,1,2,1,2,1,2,1});
	GameObject *node = new ResourceNode("Iron Rich Stone",{1.0f,1.0f},{{2,{2,1}},{100,{8,1}}},10,13);
	// Chog->work(node);
	// this->createSquad("Player squad")->add(this->playerChar);
// TBAGame->playerChar->squad->add(new Character("Archer",160,{-2+i,8},{13,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}));
	// new Character("Archer",160,{-2,9},{13,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9});
	
	Character *archer;
	Character *tamelia = new Character("Amelia",8,{-rand()%10-3,-rand()%20});
	Character *amelia;
	
	// for(int i=0;i<1;i++) {
		// for(int j=-2;j<=2;j++) {
			// archer = new Character("Archer",160,{-5+j,8+i});
			// archer->inventory->add(9,80);
			// archer->inventory->add(3,3);
			// archer->inventory->add(13);
			// archer->inventory->add(11);
			// archer->setStatus(STATUS_COMBAT);
			// TBAGame->playerChar->addToSquad(archer,true);
			// archer->setStatus(STATUS_COMBAT);
			// for(int k=0;k<2;k++) {
				// amelia = new Character("Amelia",8,{-rand()%5+2,-rand()%20});
				// amelia->inventory->add({14,15,16,17,10});
				// amelia->inventory->add({10});
				// amelia->inventory->add(3,3);
				// amelia->setTarget(archer);
				// amelia->setStatus(STATUS_COMBAT);
				// amelia->evaluateEquipment();
				// tamelia->addToSquad(amelia,true);
			// }
		// }
	// }
	
	
	
	// TBAGame->playerChar->squad->alert(amelia);
	// GameObject *node = TBAGame->gameWorld->getTileAt(1,1)->objects.at(0);
	// Character *amelia;
	// for(int i=0;i<00;i++) {
		// amelia = new Character("Amelia",8,{-rand()%10,-rand()%20});
		// amelia = new Character("Amelia",8,{-1,-1});
		// amelia->maxMoveSpeed = 4;
		// amelia->turnSpeed = 4;
		// amelia->work(node);
		// playerChar->squad->alert(amelia);
	// }
	
	// playerChar->setTarget(LB);
	// playerChar->setStatus(STATUS_COMBAT);

	debug("Total game objects: "+std::to_string(this->gameObjects.size()));
	debug("Total UI objects: "+std::to_string(this->gameUIObjects.size()));
	
	debug("Game setup complete\n");

	
	this->gameRunning = true;

	
}

//=======================
//			GAME OBJECTS
//=======================

std::vector<GameObject*> Game::convert(const std::vector<Character*> &v) {
	std::vector<GameObject*> objs;
	for(int i=0;i<v.size();i++) {
		objs.push_back(static_cast<GameObject*>(v.at(i)));
	}
	return objs;
}

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

Squad* Game::createSquad(std::string s) {
	if(s == "None") s = "Squad "+std::to_string(++this->numSquads);
	Squad* newSquad = new Squad({},s);
	this->squads.push_back(newSquad);
	return newSquad;
}

//=============
//		MISC
//=============

void Game::setPlayer(Character *c) {
	if(this->playerChar != nullptr) this->playerChar->player = false;
	this->playerChar = c;
	this->playerChar->player = true;
}

bool Game::togglePause() {
	this->paused = !this->paused;
	if(this->paused) {
		TBAGame->gameWindow->createPopup("Paused",10,true);
		// this->gameWindow->popupBox->setToggledContent("Paused");
		// this->gameWindow->popupBox->toggled = true;
	} else {
		// this->gameWindow->popupBox->toggled = false;
		TBAGame->gameWindow->deleteFirstToggledPopup();
	}
	return this->paused;
}

void Game::popupText(int duration, const std::string& message) {

	//Duration in seconds
	// this->gameWindow->popupBox->addMessage(duration*1000,message);

}

std::vector<std::string> Game::getItemNames(const std::vector<Item*> &items) {
	std::vector<std::string> itemNames;
	for(int i=0;i<items.size();i++) {
		if(items.at(i) == nullptr) {
			itemNames.push_back("");
		} else {
			itemNames.push_back(items.at(i)->getName());
		}
	}
	return itemNames;
}


//=============
//		UPDATE
//=============

void logic_thread_routine(Game *game) {
	Uint32 start;
	int elapsed;
	int real_wait = 0;
	
	TBA_Interval latencyNotif = TBA_Interval(game->latency_notif_interval);
	
	game->logicEnabled = true;
	debug("Game logic enabled");
		
	while(game->gameRunning) {

		start = SDL_GetTicks();
		game->update_logic();

		elapsed = SDL_GetTicks()-start;

		real_wait = (1000/game->logicTickRate)-elapsed;
		if(latencyNotif.check()) {
			if(real_wait < 0) {
				debug("Falling behind! (logic, "+std::to_string(-real_wait)+"ms.): "+std::to_string(game->logicTicks));
			}
		}
		game->logicPerf.push(elapsed);
		game->lastLogicUpdateTime = elapsed;

		pthread_cond_signal(&game->canUpdateGraphics);
		std::this_thread::sleep_for(std::chrono::milliseconds(real_wait));
	}
}

void graphics_thread_routine(Game *game) {
	Uint32 start;
	int elapsed;
	int real_wait = 0;
	
	TBA_Interval latencyNotif = TBA_Interval(game->latency_notif_interval);
	
	game->graphicsEnabled = true;
	debug("Graphics enabled");
			
	while(game->gameRunning) {
		
		pthread_cond_wait(&game->canUpdateGraphics,&game->graphicsLock);

		start = SDL_GetTicks();
		game->update_graphics();
		elapsed = SDL_GetTicks()-start;

		if(latencyNotif.check()) {
			if(real_wait < 0) {
				debug("Falling behind! (graphics, "+std::to_string(-real_wait)+"ms.): "+std::to_string(game->graphicsTicks));
			}
		}
		game->graphicsPerf.push(elapsed);
		game->lastGraphicsUpdateTime = elapsed;
		
	}
}

void input_thread_routine(Game* game) {
	std::string s;
	while(game->gameRunning) {
		std::cin >> s;
		debug("Received command "+s);
	}
}


void Game::spawn_threads() {
	if(pthread_create(&this->logic_thread,NULL,logic_thread_routine,this) != 0) this->gameRunning = false;
	if(pthread_create(&this->graphics_thread,NULL,graphics_thread_routine,this) != 0) this->gameRunning = false;
	if(pthread_create(&this->input_thread,NULL,input_thread_routine,this) != 0) this->gameRunning = false;
}

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

void Game::update_logic() {
	if(!this->paused) {
		this->lastLogicUpdate = SDL_GetTicks();
		this->updateGameObjects();
		this->logicTicks++;
	}
}

void Game::update_graphics() {
	this->gameWindow->update(this->debugMode);
	this->graphicsTicks++;
}

void Game::update() {
	
	// std::cout << "Active tiles: "<< this->gameWindow->mapScreen->active_tiles << "/"<< 1189 << ", " << "Frame times: " << this->lastGraphicsUpdateTime << "ms. at " << this->logicTicks << "T, " << graphicsPerf.size << "T avg: " << average(graphicsPerf.get_elements()) << "ms        \r";
	this->input();
	
	if(this->last_perf_notif + this->perf_notif_interval <= SDL_GetTicks()) {
		pthread_mutex_lock(&printLock);
		std::cout << this->logicTicks << "T @ Logic: " << this->lastLogicUpdateTime << "ms (" << this->gameObjects.size() << " objs.)" << "; Avg " << average(this->logicPerf.get_elements()) << "<" << this->logicPerf.size << ">" << ", Graphics: " << this->lastGraphicsUpdateTime << "ms; Avg " << average(this->graphicsPerf.get_elements()) << "<" << this->graphicsPerf.size << ">" << "                  \r" << std::flush;
		pthread_mutex_unlock(&printLock);
		last_perf_notif = SDL_GetTicks();
	}
	
	std::this_thread::sleep_for(std::chrono::milliseconds(36));

}

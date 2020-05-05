#include "Game.h"

#include <thread>
#include <random>
#include <memory>

#include "../ui/Window.h"
#include "../ui/Screen.h"

#include "../tools/Utility.h"
#include "../tools/Network.h"

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

#include "../../../shared/Shared.h"

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
	
	// pthread_mutex_init(&this->updateLock, NULL);
	
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
			new		Command({"connect"},connectFunc,connectEC),
			////
		};
	
	//Populate string command list with command names for autocomplete
	for(int i=0;i<this->commandList.size();i++) {
		this->commandStrings.push_back(this->commandList.at(i)->aliases.at(0));
	}

	//Length of one edge of map square
	int quadSize = 64;

	this->gameWorld = new World(quadSize*2);
	
	gameWorld->worldTexture = SDL_CreateTexture(this->gameWindow->renderer,
                               SDL_GetWindowPixelFormat( this->gameWindow->window),
								SDL_TEXTUREACCESS_TARGET,
                               gameWorld->size*this->gameWindow->mapScreen->charW,
                               gameWorld->size*this->gameWindow->mapScreen->charH);
							   
	gameWorld->screenFont->generateFontTexture(this->gameWindow->window,this->gameWindow->renderer);
	
	this->gameWorld->genWorld();
	
	//Connection signals network thread
	// Network thread signals graphics thread once it finished loading world
	// Graphics thread signals logic thread once map texture is fully generated
	
	debug("Waiting for connection...");
	
	pthread_cond_wait(&this->graphicsEnabled,&this->graphicsLock);

	this->gameWorld->genWorld_new(this->gameWindow->renderer);
	
	pthread_cond_signal(&this->logicEnabled);

	//Create player and fill inventory with generic items
	this->playerChar = new Character("Player",160,{0,0});
	this->playerChar->giveItem(2);
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

	this->gameWorld->createStructure({0,0}, ruins, 4);
	// new Container("Footlocker",{-2.0f,-2.0f},160,{3,3,3,3,3,3,3,3,4,3,1,1,2,1,2,1,2,1,2,1});
	// GameObject *node = new ResourceNode("Iron Rich Stone",{2.0f,2.0f},{{2,{2,1}},{100,{8,1}}},10,1);
	
	for(int i=0;i<0;i++) {
		// TBAGame->playerChar->squad->add(new Character("Archer",160,{-2+i,8},{13,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}));
		// TBAGame->playerChar->squad->add(new Character("Archer",160,{-2+i,9},{13,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}));
		// TBAGame->playerChar->squad->add(new Character("Archer",160,{-2+i,7},{13,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}));
		// TBAGame->playerChar->squad->add(new Character("Archer",160,{-2+i,6},{13,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}));
		// TBAGame->playerChar->squad->add(new Character("Archer",160,{-2+i,5},{13,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}));
	}

	checkHelp();
	
	checkItemTypes();
	
	debug("Total game objects: "+std::to_string(this->gameObjects.size()));
	debug("Total UI objects: "+std::to_string(this->gameUIObjects.size()));
	
	debug("Game setup complete\n");
	
	
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
	this->playerChar->player = false;
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
//		NETWORK
//=============

std::string Game::serializeInput() {
	//	00000000 00 00 000|c1|c2|c3|c4|...
	// Todo: zero-padding function
	int block = 8;
	std::string packet;
	std::string charID = std::to_string(TBAGame->playerChar->objectID);
	std::string movement = std::to_string(this->m_forward)+std::to_string(this->m_back);
	std::string turning = std::to_string(this->turn_left)+std::to_string(this->turn_right);
	std::string commands = "drink potion";
	for(int i=0;i<block;i++) {
			packet += '0';
	}
	int j=0;
	int start = packet.size()-charID.size();
	for(int i=start;i<block;i++) {
			packet[i] = charID[j++];
	}
	packet+=movement;
	packet+=turning;
	packet+=commands;
	
	return packet;
	
}

void Game::deserializeObjects(const std::string &content) {

	int i=0;
	
	while(i < content.size()) {
		std::string type;
		// i+=2;
		// debug("Unpacking content type");
		debug(content.substr(i,20));
		unpack(i,type,content,PAD_SHORT);
		// debug("Done unpacking content typ
		// debug(content.substr(0,2000));
		switch(toInt(type)) {
			case OBJ_CHARACTER: {
				// debug("Reconstructing character...");
				
				std::string id,name,x,y,movement,aim,status,targetID;
				
				std::string limbtype,hp,max;
				
				unpack(i,id,content,PAD_INT);
				unpack(i,name,content,PAD_STR);
				unpack(i,x,content,PAD_FLOAT);
				unpack(i,y,content,PAD_FLOAT);
				unpack(i,movement,content,PAD_SHORT);
				unpack(i,aim,content,PAD_FLOAT);
				unpack(i,status,content,PAD_LONG);
				unpack(i,targetID,content,PAD_INT);
				
				unpad(name);

				Character *newCharf = this->findObject(std::stoi(id));
				if(newCharf == nullptr) {
					newCharf = new Character(name,64,{toInt(x)/100.0f,toInt(y)/100.0f});
					newCharf->objectID = std::stoi(id);
				}

				newCharf->move_forward = (bool)toInt(movement.substr(0,1));
				newCharf->move_back = (bool)toInt(movement.substr(1,1));
				
				// newCharf->x = toInt(x)/100.0f;
				// newCharf->y = toInt(y)/100.0f;
				
				newCharf->targetAng = toInt(aim)/100.0f;
				newCharf->status = toFlag(status);
				
				if(std::stoi(targetID) >= 0) {
					newCharf->setTarget(TBAGame->findObject(std::stoi(targetID)));
				} else {
					newCharf->target = nullptr;
				}

				//Limbs
				for(int j=0;j<4;j++) {
					unpack(i,limbtype,content,PAD_SHORT);
					unpack(i,hp,content,PAD_INT);
					unpack(i,max,content,PAD_INT);
					newCharf->body->getLimb((LimbType)toInt(limbtype))->health = toInt(hp);
					newCharf->body->getLimb((LimbType)toInt(limbtype))->maxHealth = toInt(max);
				}
				
				//Inventory
				std::string size,itemid,uuid;
				unpack(i,size,content,PAD_INT);
				Item* newItem;
				for(int j=0;j<toInt(size);j++) {
					unpack(i,itemid,content,PAD_INT);
					unpack(i,uuid,content,PAD_INT);
					if(newCharf->inventory->find(toInt(uuid)) == -1) {
						newItem = new Item(toInt(itemid));
						newItem->UUID = toInt(uuid);
						newCharf->giveItem(newItem);
					} else {
						//Update item....
					}
				}
				// debug("Done");
				break;
			}
			case OBJ_CONTAINER: {
				// debug("Reconstructing container...");
				std::string displayid,id,name,x,y;
				unpack(i,displayid,content,PAD_SHORT);
				unpack(i,id,content,PAD_INT); 
				unpack(i,name,content,PAD_STR); 
				unpack(i,x,content,PAD_FLOAT);
				unpack(i,y,content,PAD_FLOAT); 
				
				unpad(name);
				
				Container *newContainer = this->findObject(toInt(id));
				if(newContainer == nullptr) {
					newContainer = new Container(name,{toInt(x)/100.0f,toInt(y)/100.0f},64);
					newContainer->objectID = toInt(id);
				}
				
				// debug("Done");
				break;
			}
			case OBJ_PROJECTILE: {
				// debug("Reconstructing projectile...");
				
				std::string displayid,id,active,ang,vel,x,y;
				unpack(i,displayid,content,PAD_SHORT);
				unpack(i,id,content,PAD_INT); 
				
				unpack(i,active,content,PAD_BOOL);
				unpack(i,ang,content,PAD_FLOAT);
				unpack(i,vel,content,PAD_FLOAT);
				unpack(i,x,content,PAD_FLOAT);
				unpack(i,y,content,PAD_FLOAT); 
				// debug("Done unpacking projectile info");				
				// std::unique_ptr<Projectile> newProj(this->findObject(toInt(id))->getAsProjectile());
				Projectile *newProj = this->findObject(toInt(id))->getAsProjectile();
				if(newProj == nullptr) {
					// newProj.reset(new Projectile({toInt(x)/100.0f,toInt(y)/100.0f},toInt(ang)/100.0f,toInt(vel)/100.0f));
					newProj = new Projectile({toInt(x)/100.0f,toInt(y)/100.0f},toInt(ang)/100.0f,toInt(vel)/100.0f);
					newProj->objectID = toInt(id);
				}
				// debug("Done generating projectile object");	
				if(newProj->active == false) break;
				newProj->active = (bool)toInt(active);
				newProj->x = toInt(x)/100.0f;
				newProj->y = toInt(y)/100.0f;
				newProj->updateLocation();
				newProj->angle = toInt(ang)/100.0f;
				newProj->velocity = toInt(vel)/100.0f;
				// debug("Done updating projectile object");
				break;
			}
			case OBJ_INTERACTIVE: {
				// debug("Reconstructing Resource node...");
				std::string displayid,id,name,x,y;
				unpack(i,displayid,content,PAD_SHORT);
				unpack(i,id,content,PAD_INT); 
				unpack(i,name,content,PAD_STR); 
				unpack(i,x,content,PAD_FLOAT);
				unpack(i,y,content,PAD_FLOAT); 
				
				unpad(name);
				
				ResourceNode *newNode = this->findObject(toInt(id));
				if(newNode == nullptr) {
					newNode = new ResourceNode(name,{toInt(x)/100.0f,toInt(y)/100.0f},{{2,{2,1}},{100,{8,1}}},10,1);
					newNode->objectID = toInt(id);
				}
				// debug("Done");
				break;
			}
			default:
				// debug("Unhandled object type");
				debug(content.substr(i,20));
				break;
		}
	}
	// debug("Done");
}

//=============
//		UPDATE
//=============

void logic_thread_routine(Game *game) {
	Uint32 start;
	int elapsed;
	int real_wait = 0;
	
	while(!game->client->active) {
		pthread_cond_wait(&game->logicEnabled,&game->logicLock);
	}	
	
	debug("Game logic enabled");
	
	while(game->gameRunning) {
		// debug("Updating logic");
		// pthread_mutex_lock(&game->updateLock);
		//while(game->canUpdateLogic == false) pthread_cond_wait(&game->logic,&game->updateLock);
		start = SDL_GetTicks();
		if(!game->paused) game->update_logic();
		// debug("Done updating logic");
		elapsed = SDL_GetTicks()-start;

		// pthread_mutex_unlock(&game->updateLock);

		real_wait = (1000/game->logicTickRate)-elapsed;
		if(real_wait <= 0) debug("Falling behind! (logic)");
		// debug(real_wait);
		std::this_thread::sleep_for(std::chrono::milliseconds(real_wait));
	}
}
void network_thread_routine(Game *game) {
	TBAGame->client->TBA_connect();
	while(1) {
		while(!game->client->active) {
			pthread_cond_wait(&game->networkEnabled,&game->networkLock);
			debug("Client is connected, network thread now accepting messages from server");
		}
		TBAGame->client->TBA_receive();
	}
		
}	

void Game::spawn_threads() {
	if(pthread_create(&this->network_thread,NULL,network_thread_routine,this) != 0) this->gameRunning = false;
	if(pthread_create(&this->logic_thread,NULL,logic_thread_routine,this) != 0) this->gameRunning = false;
}

void Game::updateGameObjects() {
	// pthread_mutex_lock(&this->logicLock);
	if(this->needsUpdate) {
		// debug("Unpacking game objects");
		pthread_mutex_lock(&this->networkLock);
		this->deserializeObjects(this->objectUpdates);
		this->objectUpdates = "";
		this->needsUpdate = false;
		// debug("Done unpacking objects");
		pthread_mutex_unlock(&this->networkLock);
	}
	
	// debug("Finished importing game objects");

	std::vector<Tile*> nearbyTiles = this->gameWorld->getTilesInRadius(this->playerChar->x,this->playerChar->y,15);
	Tile* thisTile;
	
	for(int i=0;i<nearbyTiles.size();i++) {
		thisTile = nearbyTiles.at(i);
		// this->gameObjects.at(i)->update();
		if(thisTile->isOccupied()) {
			for(int j=0;j<thisTile->occupiers.size();j++) {
				thisTile->occupiers.at(j)->update();
			}
			// debug("Done updating character");
		}
		
		if(thisTile->hasObjects()) {
			for(int j=0;j<thisTile->objects.size();j++) {
				thisTile->objects.at(j)->update();
			}
			// debug("Done updating objects");
		}
	}
	// debug("Finish");
	// pthread_mutex_unlock(&this->logicLock);
}

void Game::updateGameUIObjects() {
	for(int i=0;i<this->gameUIObjects.size();i++) {
		this->gameUIObjects.at(i)->update();
	}
}

void Game::update_logic() {

	//Suspend logic ticks if game is paused
	//	if(SDL_GetTicks() >= this->lastLogicUpdate + (1000/this->logicTickRate)) {
	//Update all active game objects
	this->lastLogicUpdate = SDL_GetTicks();
	int start = SDL_GetTicks();
	if(!this->paused) {
		this->updateGameObjects();
		this->logicTicks++;
	}
	// debug("Done updating graphics");
	int elapsed = SDL_GetTicks()-start;
	
	// pthread_mutex_unlock(&game->updateLock);

	int real_wait = (1000/this->logicTickRate)-elapsed;
	// if(real_wait <= 0) debug("Falling behind! (logic)");
	// SDL_Delay(real_wait);
	
	//this->timeToNextLogicUpdate = (this->lastLogicUpdate + (1000/this->logicTickRate)) - SDL_GetTicks();
	//	}
	

}

void Game::update_graphics() {
	//if(SDL_GetTicks() >= this->lastGraphicsUpdate + (1000/this->graphicsTickRate)) {
		//Update game window and all screens
	// int start = SDL_GetTicks();
	this->gameWindow->update(this->debugMode);
	this->graphicsTicks++;
	// debug("Done updating graphics");
	// int elapsed = SDL_GetTicks()-start;
	
	// pthread_mutex_unlock(&game->updateLock);

	// int real_wait = (1000/this->graphicsTickRate)-elapsed;
	// if(real_wait <= 0) debug("Falling behind! (graphics)");
	// SDL_Delay(real_wait);
		//this->timeToNextGraphicsUpdate = (this->lastGraphicsUpdate + (1000/this->graphicsTickRate)) - SDL_GetTicks();
	//}

}

void Game::update() {
	
	// std::cout << "Game objects: " << this->gameObjects.size() << "\r" << std::flush;

	int start = SDL_GetTicks();
	this->update_graphics();

	int elapsed = SDL_GetTicks()-start;

	int real_wait = (1000/this->graphicsTickRate)-elapsed;
	if(real_wait <= 0) debug("Falling behind! (graphics)");
	// SDL_Delay(real_wait);
	std::this_thread::sleep_for(std::chrono::milliseconds(real_wait));


}

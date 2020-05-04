#include "Game.h"

#include <thread>
#include <random>

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

//=============
//	 	SETUP
//=============

void Game::setupGame() {
	
	this->gameLog = new Log("Game.txt");
	gameLog->writeln("Beginning game startup\n");
	
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
			new		Command({"server"},serverFunc,serverEC),
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
	
	// gameWorld->worldTexture = SDL_CreateTexture(this->gameWindow->renderer,
                               // SDL_GetWindowPixelFormat( this->gameWindow->window),
								// SDL_TEXTUREACCESS_TARGET,
                               // gameWorld->size*this->gameWindow->mapScreen->charW,
                               // gameWorld->size*this->gameWindow->mapScreen->charH);
							   
	// gameWorld->screenFont->generateFontTexture(this->gameWindow->window,this->gameWindow->renderer);
	this->gameWorld->genWorld();
		// this->gameWorld->genWorld_new(this->gameWindow->renderer);

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
	
	//New characters are added to gameObjects automatically
	Character *newChar,*LB,*Dog,*Chog;
	for(int i=0;i<0;i++) {
		// newChar = new Character("Looter "+std::to_string(i+1),(rand()%(1+(quadSize*2)))-quadSize,(rand()%(1+(quadSize*2)))-quadSize);
		if(rand()%2 == 0) newChar->equipment->equip(new Item(4),EQUIP_PRIMARY);
		newChar->setTarget(this->playerChar);
		newChar->setStatus(STATUS_COMBAT);
		//newChar->setTarget(this->playerChar);
		//new Character(false,160,"Looter",-quadSize+i+1,-quadSize+1+(i/quadSize));
	}

	newChar = new Character("Debug Trader",160,{0,-7});
	Chog = new Character("Chog",160,{0,-7});

	// TBAGame->setDisplayTarget(newChar);
	// newChar->moveTo(0,0);
	newChar->maxMoveSpeed = this->playerChar->maxMoveSpeed*2;
	LB = new Character("Lost Bladesman",160,{0,1});
	LB->setTarget(this->playerChar);
	LB->setStatus(STATUS_COMBAT);
	Dog = new Character("Wolf",160,{5,5});
	Dog->equipment->equip(new Item(13),EQUIP_PRIMARY);
	Dog->giveItem(8);
	Dog->giveItem(8);
	Dog->giveItem(8);
	Dog->giveItem(8);

	// LB->equipment->primary = new Item(4);
	Dog->maxMoveSpeed = playerChar->maxMoveSpeed*2;
	Dog->turnSpeed = playerChar->turnSpeed*2;
	// newChar->lookAt(LB);
	//newChar->setTarget(LB);
	Dog->setTarget(LB);
	Dog->setStatus(STATUS_COMBAT);
	//newChar->equipment->primary = new Item(4);
	//newChar->setTarget(playerChar);
	//newChar->setStatus(STATUS_COMBAT);
	//static_cast<Character*>(this->gameObjects.at(2))->setTarget(newChar);
	//static_cast<Character*>(this->gameObjects.at(2))->setStatus(STATUS_COMBAT);	
	this->gameWorld->createStructure({0,0}, ruins, 4);
	new Container("Footlocker",{-2.0f,-2.0f},160,{3,3,3,3,3,3,3,3,4,3,1,1,2,1,2,1,2,1,2,1});
	GameObject *node = new ResourceNode("Iron Rich Stone",{2.0f,2.0f},{{2,{2,1}},{100,{8,1}}},10,1);
	this->createSquad("Player squad")->add(this->playerChar);

	for(int i=0;i<5;i++) {
		TBAGame->playerChar->squad->add(new Character("Archer",160,{-2+i,8},{13,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}));
		TBAGame->playerChar->squad->add(new Character("Archer",160,{-2+i,9},{13,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}));
		TBAGame->playerChar->squad->add(new Character("Archer",160,{-2+i,7},{13,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}));
		TBAGame->playerChar->squad->add(new Character("Archer",160,{-2+i,6},{13,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}));
		TBAGame->playerChar->squad->add(new Character("Archer",160,{-2+i,5},{13,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}));
		LB->addToSquad(new Character("LB Archer",160,{-2+i,10},{13,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}),true);
		LB->addToSquad(new Character("LB Archer",160,{-2+i,12},{13,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}),true);
	}
	
	checkHelp();
	checkItemTypes();

	debug("Total game objects: "+std::to_string(this->gameObjects.size()));
	debug("Total UI objects: "+std::to_string(this->gameUIObjects.size()));
	
	debug("Game setup complete\n");
	
	debug("Creating game server");
	this->server = new Server(serverLock);
	debug("Game server setup complete\n");
	
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
	this->playerChar->player = false;
	this->playerChar = c;
	this->playerChar->player = true;
}

bool Game::togglePause() {
	this->paused = !this->paused;
	if(this->paused) {
		// TBAGame->gameWindow->createPopup("Paused",10,true);
		// this->gameWindow->popupBox->setToggledContent("Paused");
		// this->gameWindow->popupBox->toggled = true;
	} else {
		// this->gameWindow->popupBox->toggled = false;
		// TBAGame->gameWindow->deleteFirstToggledPopup();
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

void Client::deserialize() {
	//	00000000 00 00 000|c1|c2|c3|c4|...
	int block = 8;
	std::string charID;
	std::string movement;
	std::string turning;
	std::string commands;
	for(int i=0;i<0+block;i++) {
		charID += this->packet[i];
	}
	for(int i=8;i<8+2;i++) {
		movement += this->packet[i];
	}
	for(int i=10;i<10+2;i++) {
		turning += this->packet[i];
	}
	for(int i=12;i<packet.size();i++) {
		commands += this->packet[i];
	}
	this->characterID = std::stoi(charID);
	for(int i=0;i<split('|',commands).size();i++) {
		this->pendingCommands.push(split('|',commands).at(i));
	}
	this->info = {std::stoi(movement),std::stoi(turning)};
}

void Game::processClientUpdates(Client *c) {
	c->deserialize();
	//do stuff with c->commands etc.
	//We should be reading from and updating the client constantly so shouldn't set/reset a bool 
	c->hasNewPacket = false;
}

void network_thread_routine(Game *game) {
	Uint32 start;
	int elapsed;
	int real_wait = 0;
	
	int newSocket;
	
	debug("Networking enabled");
	
	game->server->TBA_start();
	
	//If all client slots are taken this should technically be waiting for a signal from claimedSocket
	
	while(1) {
		newSocket = game->server->TBA_accept();
		TBAGame->server->push_socket_queue(newSocket);
		
		//
		// Should look up characters in some registry here
		//
		// Perform new connection actions here
		//		
		TBAGame->clients.insert({newSocket,new Client()});
		
		pthread_cond_signal(&freeSocket);
	}
}

void await_socket(ClientThread* worker) {
	
	pthread_mutex_lock(&workerLock);
	int client;
	while(TBAGame->server->connections.size() == 0) {
		pthread_cond_wait(&freeSocket,&workerLock);
	}
	worker->client = TBAGame->server->pop_socket_queue();
	worker->active = true;
	pthread_cond_signal(&claimedSocket);
	pthread_mutex_unlock(&workerLock);
}

void worker_thread_routine(ClientThread *worker) {
	while(1) {
		await_socket(worker);
		debug("Received new connection on client thread "+std::to_string(worker->id));
		for(int i=0;i<TBAGame->workers->size;i++) {
			if(TBAGame->workers->getThread(i)->active) {
				debug("Slot "+std::to_string(i)+": "+std::to_string(TBAGame->workers->getThread(i)->client));
			} else {
				debug("Slot "+std::to_string(i)+": -");
			}
		}

		//Send client world and object info on initial connection
		TBAGame->server->TBA_send(worker->client,TBAGame->gameWorld->serialize());
		// std::this_thread::sleep_for(std::chrono::milliseconds(100));
		TBAGame->server->TBA_send(worker->client,TBAGame->serializeObjects());
		
		// while(TBAGame->server->TBA_service(worker->client)) {
		while(1) {
			TBAGame->server->TBA_service(worker->client);
			if(!TBAGame->server->TBA_send(worker->client,TBAGame->serializeObjects())) break;
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		std::cout << "Server slot " << worker->id << " freed" << std::endl;
		worker->active = false;
	}
}

void logic_thread_routine(Game *game) {
	Uint32 start;
	int elapsed;
	int real_wait = 0;
	
	//Wait for everything to start up
	std::this_thread::sleep_for(std::chrono::milliseconds(30));
	
	TBAGame->displayText("Game logic enabled");
	
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

void Game::spawn_threads() {
	if(pthread_create(&network_thread,NULL,network_thread_routine,this) != 0) this->gameRunning = false;
	if(pthread_create(&logic_thread,NULL,logic_thread_routine,this) != 0) this->gameRunning = false;

	if(!this->workers->initialize(worker_thread_routine)) this->gameRunning = false;
	debug("All threads spawned successfully");
}

//=============
//		UPDATE
//=============

void Game::updateGameObjects() {
	for(std::map<int,Client*>::iterator it = this->clients.begin();it != this->clients.end();it++) {
		if(it->second->hasNewPacket) {
			this->processClientUpdates(it->second);
		}
	}
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

void Game::update() {
	
	// std::cout << "Game objects: " << this->gameObjects.size() << "\r" << std::flush;

	int start = SDL_GetTicks();
	// this->update_graphics();

	int elapsed = SDL_GetTicks()-start;

	int real_wait = (1000/this->graphicsTickRate)-elapsed;
	if(real_wait <= 0) debug("Falling behind! (graphics)");
	// SDL_Delay(real_wait);
	std::this_thread::sleep_for(std::chrono::milliseconds(real_wait));


}

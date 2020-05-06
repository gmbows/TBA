#include "Network.h"
#include "../common/Common.h"
#include "../../../shared/Shared.h"
#include <unistd.h>

void Client::dispatchPacket(int type) {
	// std::cout << "[Got " << this->packetBuffer.size() << " bytes from server]" << std::endl;
	switch(type) {
		case SERV_MSG:
			std::cout << "[SERV] " << this->packetBuffer << std::endl;
			break;
		case DATA_WORLD:
			// break;
			// std::cout << "[Data type: World]" << std::endl;
			TBAGame->gameWorld->deserialize(this->packetBuffer);
			pthread_cond_signal(&TBAGame->graphicsEnabled);
			break;
		case DATA_OBJS:
			// break;
			// std::cout << "[Data type: Objects]" << std::endl;
			pthread_mutex_lock(&TBAGame->networkLock);
			TBAGame->objectUpdates += this->packetBuffer;
			pthread_mutex_unlock(&TBAGame->networkLock);
			TBAGame->needsUpdate = true;
			break;
		case EVENT_PROJ_COLLIDE:
			// break;
		case EVENT_PROJ_CREATE:
			// std::cout << "[Data type: Event]" << std::endl;
			// break;
			pthread_mutex_lock(&TBAGame->networkLock);
			TBAGame->objectUpdates += this->packetBuffer;
			pthread_mutex_unlock(&TBAGame->networkLock);
			TBAGame->needsUpdate = true;
			break;
		default:
			// std::cout << "[Data type: " << type << " unknown]" << std::endl;
			break;
	}
	// this->packetBuffer.clear();
	// debug("[Dispatch complete]\n");
	this->TBA_send("ACK");
}

void Client::handlePacket(char* buf) {
	
	std::string packet;
	copyBuf(buf,PACKET_SIZE,packet);
	
	if(packet.size() > PACKET_SIZE) {
		int diff = packet.size()-PACKET_SIZE;
		packet.erase(packet.size()-diff,diff);
	}
	
	if(packet.size() == 0) return;
	
	if(remaining == 0) {
		remaining = std::stoi(packet.substr(0,PAD_INT));
		packet.erase(0,PAD_INT);
	}
	
	if(remaining <= packet.size()) {
		
		this->packetBuffer += packet.substr(0,remaining);
		
		int type = std::stoi(this->packetBuffer.substr(0,2));
		
		this->packetBuffer.erase(0,2);
		
		this->dispatchPacket(type);
		
		packet.erase(0,remaining);
		
		this->packetBuffer = packet;
		remaining = 0;
	} else {
		this->packetBuffer += packet;
		remaining -= packet.size();
	}

}

bool Client::TBA_receive() {
	int err;
	int size;
	char buffer[PACKET_SIZE];
	
	std::string content;
	
	std::string packet = "";
	std::string stringBuf = "";
	bool reading = false;
	
	while(err = recv(this->server, buffer, sizeof(buffer),0) > 0) {
		this->handlePacket(buffer);
		memset(buffer, 0, sizeof(buffer));
	}
	//connection closed gracefully
	if(err == 0) {
		std::cout << "Received disconnect from server" << std::endl;
		this->active = false;
		return false;
	//connection closed ungracefully
	} else if(err == SOCKET_ERROR) {
		std::cout << "Unhandled socket error: " << WSAGetLastError() << ", closing" << std::endl;
		closesocket(this->server);
		return false;
	} else {
		return true;
	}
}

bool Client::TBA_send(const std::string &s) {
	if(!this->active) {
		std::cout << "Not connected to a server" << std::endl;
		return false;
	}

	int err = send(this->server, s.c_str(), sizeof(s), 0);

	if(err == -1) {
		std::cout << "Failed to send \"" << s << "\" to server over socket " << this->server << " with error code " << WSAGetLastError() << std::endl;
		return false;
	} else {
		if(s == "ACK") return true;
		std::cout << "Sent \"" << s << "\" to server over socket " << this->server << std::endl;
		return true;
	}
}

bool Client::TBA_connect() {
	// connect(this->server, (SOCKADDR *)&addr, sizeof(addr));
	this->server = socket(AF_INET, SOCK_STREAM, 0);
	char greeting[PACKET_SIZE];
	if(connect(this->server, (SOCKADDR *)&addr, sizeof(addr)) == 0) {
		// read(this->server,greeting,sizeof(greeting));
		std::cout << "Connected to server on socket " << this->server << std::endl;
		// std::string greet(greeting);	
		// debug(greeting);
		pthread_cond_signal(&TBAGame->networkEnabled);
		this->active = true;
		return true;
	} else {
		std::cout << "Connection failed with error code " << WSAGetLastError() << std::endl;
		return false;
	}
}
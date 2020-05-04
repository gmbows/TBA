#include "Network.h"
#include "../common/Common.h"
#include "../../../shared/Shared.h"
#include <unistd.h>

void Client::dispatchPacket(int type) {
	std::cout << "[Got " << this->packetBuffer.size() << " bytes from server]" << std::endl;
	switch(type) {
		case DATA_WORLD:
			std::cout << "[Data type: World]" << std::endl;
			TBAGame->gameWorld->deserialize(this->packetBuffer);
			pthread_cond_signal(&TBAGame->graphicsEnabled);
			// TBAGame->gameWorld->genWorld_new(TBAGame->gameWindow->renderer);
			break;
		case DATA_OBJS:
			std::cout << "[Data type: Object]" << std::endl;
			pthread_mutex_lock(&TBAGame->networkLock);
			TBAGame->objectUpdates = this->packetBuffer;
			pthread_mutex_unlock(&TBAGame->networkLock);
			TBAGame->needsUpdate = true;
			// pthread_cond_signal(&TBAGame->graphicsEnabled);
			break;
		default:
			std::cout << "[Data type: " << type << " unknown]" << std::endl;
			break;
	}
	this->TBA_send("ACK");
	debug("[Dispatch complete]\n");
}

void Client::handlePacket(char* buf) {
	
	std::string packet;
	copyBuf(buf,PACKET_SIZE,packet);
	
	if(packet.size() > PACKET_SIZE) {
		int diff = packet.size()-PACKET_SIZE;
		packet.erase(packet.size()-diff,diff);
	}
	
	int size = packet.size();
	
	if(size >= STREAM_INFO_LEN) {
		if(packet.substr(0,STREAM_INFO_LEN) == DATA_BEGIN) {
			std::cout << "[Received new data stream]" << std::endl;
			packet.erase(0,STREAM_INFO_LEN);
			this->reading = true;
		}
		if(this->reading and packet.substr(packet.size()-STREAM_INFO_LEN,STREAM_INFO_LEN) == DATA_TERM) {
			packet.erase(packet.size()-STREAM_INFO_LEN,STREAM_INFO_LEN);
			// std::cout << "RECEIVED PACKET SEGMENT OF SIZE " << packet.size() << std::endl;
			// std::cout << "DATA STREAM TERMINATED" << std::endl;
			//Do something with content....
			
			this->packetBuffer += packet;
			
			int type = std::stoi(this->packetBuffer.substr(0,2));
			
			this->packetBuffer.erase(0,2);
			
			this->dispatchPacket(type);
			this->packetBuffer = "";
			this->reading = false;
			return;
		}
	}

	if(this->reading) {
		this->packetBuffer += packet;
		// std::cout << "RECEIVED PACKET SEGMENT OF SIZE " << packet.size() << std::endl;
	} else {
		if(packet.size() > 0) std::cout << "[SERV] " << packet << std::endl;
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
		read(this->server,greeting,sizeof(greeting));
		std::cout << "Connected to server on socket " << this->server << std::endl;
		std::string greet(greeting);	
		debug(greeting);
		pthread_cond_signal(&TBAGame->networkEnabled);
		this->active = true;
		return true;
	} else {
		std::cout << "Connection failed with error code " << WSAGetLastError() << std::endl;
		return false;
	}
}
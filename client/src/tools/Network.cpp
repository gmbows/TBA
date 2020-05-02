#include "Network.h"
#include "../common/Common.h"
#include "../../../shared/Shared.h"
#include <unistd.h>

bool Client::TBA_receive() {
	int err;
	int size;
	char buffer[262144];
	
	std::string content;
	
	// bool reading = false;
	
	while(err = recv(this->server, buffer, sizeof(buffer),0) > 0) {
		std::string packet(buffer);
		size = std::min((int)packet.size(),262144);
		if(size >= std::min(DATA_BEGIN_LEN,DATA_TERM_LEN)) {
			if(packet.substr(0,DATA_BEGIN_LEN) == DATA_BEGIN) {
				std::cout << "RECEIVED NEW PACKET OF SIZE " << packet.size() << std::endl;
				// reading = true;
				packet = packet.substr(DATA_BEGIN_LEN,size);
			}
			if(packet.substr(packet.size()-DATA_TERM_LEN,DATA_TERM_LEN) == DATA_TERM) {
				// std::cout << "DATA STREAM TERMINATED" << std::endl;
				//Do something with content....
				content += packet.substr(0,packet.size()-DATA_TERM_LEN);
				std::string type = content.substr(0,2);
				content = content.substr(2,content.size()-2);
				switch(std::stoi(type)) {
					case DATA_WORLD:
						std::cout << "DATA TYPE: WORLD" << std::endl;
						TBAGame->gameWorld->deserialize(content);
						pthread_cond_signal(&TBAGame->graphicsEnabled);
						// TBAGame->gameWorld->genWorld_new(TBAGame->gameWindow->renderer);
						break;
					default:
						std::cout << "DATA TYPE: UNKNOWN" << std::endl;
						break;
				}
				// reading = false;
			}
			// if(reading) {
				// std::cout << "RECEIVED PACKET OF SIZE " << size << std::endl;
				// content += packet;
			// }
		}
		//Ignore junk data
		memset(buffer, 0, sizeof(buffer));
	}
	//connection closed gracefully
	if(err == 0) {
		// std::cout << "Read 0 bytes or EOF reached" << std::endl;
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
		std::cout << "Sent \"" << s << "\" to server over socket " << this->server << std::endl;
		return true;
	}
}

bool Client::TBA_connect() {
	// connect(this->server, (SOCKADDR *)&addr, sizeof(addr));
	this->server = socket(AF_INET, SOCK_STREAM, 0);
	if(connect(this->server, (SOCKADDR *)&addr, sizeof(addr)) == 0) {
		std::cout << "Connected to server on socket " << this->server << std::endl;
		pthread_cond_signal(&TBAGame->networkEnabled);
		this->active = true;
		return true;
	} else {
		std::cout << "Connection failed with error code " << WSAGetLastError() << std::endl;
		return false;
	}
}
#include "Network.h"
#include "../common/Common.h"
#include "../../../shared/Shared.h"
#include <unistd.h>
#include <pthread.h>

int Server::pop_socket_queue() {

	int socket = this->connections.front();
	this->connections.pop();

	return socket;
}

void Server::push_socket_queue(SOCKET socket) {

	this->connections.push(socket);

}

void Server::safe_print(const std::string &s) {
	pthread_mutex_lock(&this->serverLock);
	std::cout << s << std::endl; 
	pthread_mutex_unlock(&this->serverLock);
}

void Server::TBA_start() {
	bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	
	std::cout << "Listening on " << htons(this->serverAddr.sin_port) << "..." << std::endl;
	listen(this->server, 3);
	this->active = true;
	
}

void Server::TBA_close(SOCKET client) {
	closesocket(client);
}

int Server::TBA_accept() {
	SOCKET client;
	std::cout << "Awaiting connection..." << std::endl;
	client = accept(this->server, (SOCKADDR *)&this->clientAddr, &this->clientAddrSize);
	if(client < 0) {
		std::cout << "SOCK ERROR" << std::endl;
	} else {
		std::cout << "Received new connection on socket " << client << std::endl;
	}
	return client;
}

bool Server::TBA_send(SOCKET client,const std::string &content) {

	char c[262144];
	for(int i=0;i<content.size();i++) {
		c[i] = content[i];
	}

	int err = send(client, c, sizeof(c), 0);

	if(err == -1) {
		std::cout << "Failed to send content with size " << sizeof(c) << " to client over socket " << client << " with error code " << WSAGetLastError() << std::endl;
		return false;
	} else {
		std::cout << "Sent content with size " << sizeof(c) << " to client over socket " << client << std::endl;
		return true;
	}
}

bool Server::TBA_service(SOCKET client) {
	int err;
	char buffer[1024];
	while(err = recv(client, buffer, sizeof(buffer),0) > 0) {
		std::cout << "Received packet from " << client << " with size " << sizeof(buffer) << std::endl;
		std::string packet(buffer);
		if(TBAGame->clients.find(client) == TBAGame->clients.end()) {
			debug("TBA_service(): Error: Read packet from invalid or disconnected client");
			closesocket(client);
			return false;
		} else {
			//update packet for client on this socket
			TBAGame->clients.at(client)->packet = packet;
			TBAGame->clients.at(client)->hasNewPacket = true;
		}
		memset(buffer, 0, sizeof(buffer));
	}
	//connection closed gracefully
	if(err == 0) {
		std::cout << "Socket " << client << " closed by host" << std::endl;
		closesocket(client);
		return false;
	//connection closed ungracefully
	} else if(err == SOCKET_ERROR) {
		std::cout << "Unhandled socket error: " << WSAGetLastError()<< ", closing" << std::endl;
		closesocket(client);
		return false;
	} else {
		return true;
	}
}

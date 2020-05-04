#include "Network.h"
#include "../common/Common.h"
#include "../../../shared/Shared.h"
#include <unistd.h>
#include <pthread.h>
#include <sstream>

int Server::pop_socket_queue() {

	int socket = this->connections.front();
	this->connections.pop();

	return socket;
}

void Server::push_socket_queue(SOCKET socket) {

	this->connections.push(socket);

}

bool Server::isFull() {
	return TBAGame->workers->full();
}

void Server::safe_print(const std::string &s) {
	pthread_mutex_lock(&serverLock);
	std::cout << s << std::endl; 
	pthread_mutex_unlock(&serverLock);
}

void Server::TBA_start() {
	bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	
	std::cout << "Listening on " << htons(this->serverAddr.sin_port) << "..." << std::endl;
	std::cout << "Using packet size " << PACKET_SIZE << std::endl;
	// std::basic_ostream<char> f;
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
		std::cout << "[Received new connection on socket " << client << "]" << std::endl;
	}
	if(!this->isFull()) {
		this->TBA_send(client,"Connection successful");
	} else {
		this->TBA_send(client,"Server full, please wait");
	}
	return client;
}

bool Server::TBA_send(SOCKET client,std::string content) {

	char c[PACKET_SIZE];
	int i = 0;
	int err;
	int packets = 0;
	int initial_size = content.size();
	while(content.size() > 0) {
		memset(c, 0, sizeof(c));
		i=0;
		for(i;i<std::min((int)content.size(),PACKET_SIZE);i++) {
			c[i] = content[i];
		}
		if(err = send(client, c, sizeof(c), 0) == -1) {
			std::cout << "[Failed to send packet <" << packets << "> to socket " << client << " aborting transfer]" << std::endl;
			return false;
		}
		content.erase(0,i);
		packets++;
	}
	
	std::cout << "[Sent " << initial_size << "<" << packets << ">" << " bytes over socket " << client << "]" << std::endl;
	return true;

}

bool Server::TBA_service(SOCKET client) {
	int err;
	char buffer[PACKET_SIZE];
	if(err = recv(client, buffer, sizeof(buffer),0) > 0) {
		// std::cout << "Received packet from " << client << " with size " << sizeof(buffer) << std::endl;
		// std::string packet(buffer);
		if(TBAGame->clients.find(client) == TBAGame->clients.end()) {
			debug("TBA_service(): Error: Read packet from invalid or disconnected client");
			closesocket(client);
			return false;
		} else {
			//update packet for client on this socket
			// TBAGame->clients.at(client)->packet = packet;
			// TBAGame->clients.at(client)->hasNewPacket = true;
		}
		memset(buffer, 0, sizeof(buffer));
	}
	//connection closed gracefully
	if(err == 0) {
		std::cout << "[Socket " << client << " closed by client]" << std::endl;
		closesocket(client);
		return false;
	//connection closed ungracefully
	} else if(err == SOCKET_ERROR) {
		std::cout << "[Unhandled socket error: " << WSAGetLastError()<< ", closing]" << std::endl;
		closesocket(client);
		return false;
	} else {
		return true;
	}
}
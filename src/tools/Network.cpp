#include "Network.h"
#include <unistd.h>

void Server::TBA_start() {
	bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	
	std::cout << "Listening on " << htons(this->serverAddr.sin_port) << "..." << std::endl;
	listen(this->server, 3);
	this->active = true;
}

void Server::TBA_close(int cid) {
	closesocket(cid);
	std::cout << "Closed socket " << cid << std::endl;
}

void Server::TBA_accept() {
	if(1) {
		std::cout << "Awaiting connection..." << std::endl;
		this->client = accept(this->server, (SOCKADDR *)&clientAddr, &clientAddrSize);
		if(this->client < 0) {
			std::cout << "SOCK ERROR" << std::endl;
			return;
		}
		std::cout << "Client connected" << std::endl;
		while(read(this->client, this->buffer, sizeof(this->buffer)) > 0) {
			std::cout << "Client: " << this->buffer << std::endl;
			memset(this->buffer, 0, sizeof(this->buffer));
		}
		TBA_close(this->client);
	}
}

void Client::TBA_send() {
	std::string msg;
	while(msg != "bye") {

		msg = "BRUHBRUHBRUH";
		if(!this->active) send(this->server, msg.c_str(), sizeof(msg), 0);
		msg = "bye";

	}
	closesocket(this->server);

	std::cout << "Socket closed." << std::endl;
}

void Client::TBA_connect() {
	// connect(this->server, (SOCKADDR *)&addr, sizeof(addr));
	this->server = socket(AF_INET, SOCK_STREAM, 0);
	if(connect(this->server, (SOCKADDR *)&addr, sizeof(addr)) == 0) {
		std::cout << "Connected to server on socket " << this->server << std::endl;
		this->active = true;
	} else {
		std::cout << "Connection failed with error code " << WSAGetLastError() << std::endl;
	}
}
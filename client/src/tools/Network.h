#pragma once
#include <iostream>
#include <winsock2.h>

struct Client {
	WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
 

	bool TBA_send(const std::string&);	
	bool TBA_receive();
	
	bool TBA_connect();
	
	void TBA_disconnect() {
		closesocket(this->server);
		std::cout << "Disconnected from server" << std::endl;
	}
	
	bool active = false;
	
	Client() {
		
		WSAStartup(MAKEWORD(2,0), &WSAData);
		
		addr.sin_addr.s_addr = inet_addr("74.103.180.136"); 
		addr.sin_family = AF_INET;
		addr.sin_port = htons(5555);
	}
};
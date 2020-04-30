#pragma once
#include <iostream>
#include <winsock2.h>

struct Client {
	WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
 

	void TBA_send();
	
	void TBA_connect();
	
	bool active = false;
	
	Client() {
		
		WSAStartup(MAKEWORD(2,0), &WSAData);
		server = socket(AF_INET, SOCK_STREAM, 0);
		
		addr.sin_addr.s_addr = inet_addr("10.0.0.2"); 
		addr.sin_family = AF_INET;
		addr.sin_port = htons(5555);
	}
};

struct Server {
    WSADATA WSAData;
 
    SOCKET server, client;
 
    SOCKADDR_IN serverAddr, clientAddr;
 
    char buffer[1024];
    int clientAddrSize = sizeof(clientAddr);
	
	bool active = false;

	void TBA_accept();
	 
	 void TBA_close(int cid);
	 
	 void TBA_start();
	
	Server() {
		WSAStartup(MAKEWORD(2,0), &this->WSAData);
		this->server = socket(AF_INET, SOCK_STREAM, 0);
	 
		this->serverAddr.sin_addr.s_addr = INADDR_ANY;
		this->serverAddr.sin_family = AF_INET;
		this->serverAddr.sin_port = htons(5555);
	}
};
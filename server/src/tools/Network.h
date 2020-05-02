#pragma once

#include <iostream>
#include <winsock2.h>
#include <pthread.h>
#include <queue>

struct ClientThread {
	int active = false;
	int client;
	
	pthread_t thread;
	
	void client_thread_routine() {}
	
};

struct Server {
    WSADATA WSAData;
 
    SOCKET server;
 
    SOCKADDR_IN serverAddr, clientAddr;
 
    int clientAddrSize = sizeof(clientAddr);
	
	std::queue<SOCKET> connections;
	
	void push_socket_queue(SOCKET);
	int pop_socket_queue();
	
	bool active = false;

	int TBA_accept();
	bool TBA_service(SOCKET);
	bool TBA_send(SOCKET,const std::string&);
	 
	void TBA_close(SOCKET);
	 
	void TBA_start();
	 
	void safe_print(const std::string&);
	 
	pthread_mutex_t serverLock;
	
	Server(pthread_mutex_t lock) {
		WSAStartup(MAKEWORD(2,0), &this->WSAData);
		this->server = socket(AF_INET, SOCK_STREAM, 0);
	 
		this->serverAddr.sin_addr.s_addr = INADDR_ANY;
		this->serverAddr.sin_family = AF_INET;
		this->serverAddr.sin_port = htons(5555);
	}
};

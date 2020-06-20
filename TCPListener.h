#pragma once

#include <string>
#include <thread>
#include <vector>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

class TCPListener;

typedef void(*MessageReceivedHandler)(TCPListener* listener, int socketId, std::string msg);

class TCPListener
{

public:

	TCPListener(std::string ipAddress, int port, MessageReceivedHandler handler);
	~TCPListener();

	bool Init();											// Initialize Winsock

	void Send(int clientSocket, std::string message);		// Send a message to the client
	void Run();												// Main Loop
	void ClientThread(SOCKET data);
	void Cleanup();											// Destroy the Winsock object

private:

	SOCKET CreateSocket();									// Create a socket
	SOCKET WaitForConnection(SOCKET listening);				// Wait for a connection

	inline std::thread StartSocketThread(SOCKET data) { return std::thread([=]() { this->ClientThread(data); }); }

	std::string					m_ipAddress;
	int							m_port;
	MessageReceivedHandler		m_handler;
};




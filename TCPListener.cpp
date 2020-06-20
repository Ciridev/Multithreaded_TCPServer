#include <iostream>
#include <string>
#include <thread>

#include "TCPListener.h"

#define MAX_BUFFER_SIZE (4096)

TCPListener::TCPListener(std::string ipAddress, int port, MessageReceivedHandler handler)
	: m_ipAddress(ipAddress), m_port(port), m_handler(handler)
{
}

TCPListener::~TCPListener()
{
	Cleanup();
}

bool TCPListener::Init()
{
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);

	int wsInit = WSAStartup(ver, &data);

	return wsInit == 0;
}

void TCPListener::Send(int clientSocket, std::string message)
{
	send(clientSocket, message.c_str(), message.size() + 1, 0);
}

void TCPListener::Run()
{
	bool flag = 0;
	std::thread thread_obj;

	while (!flag)
	{
		SOCKET Listening = CreateSocket();

		if (Listening == INVALID_SOCKET)
		{
			flag = 1;
		}
		else
		{
			SOCKET client = WaitForConnection(Listening);
			thread_obj = this->StartSocketThread(client);


			thread_obj.detach();
			closesocket(Listening);
		}
	}
}

void TCPListener::ClientThread(SOCKET data)
{
	SOCKET client = data;

	char buffer[MAX_BUFFER_SIZE];

	if (client != INVALID_SOCKET)
	{
		int bytesReceived = 0;
		do
		{
			ZeroMemory(buffer, MAX_BUFFER_SIZE);

			bytesReceived = recv(client, buffer, sizeof(buffer), 0);

			if (bytesReceived > 0)
			{
				if (this->m_handler != nullptr)
				{
					this->m_handler(this, client, std::string(buffer, 0, bytesReceived));

					std::cout << "Received from client: " << std::string(buffer, 0, bytesReceived) << std::endl;
				}
			}
			else if (bytesReceived == SOCKET_ERROR)
			{
				std::cerr << "Error in Receiving data." << std::endl;
				return;
			}
			else
			{
				std::cerr << "Client disconnect." << std::endl;
				return;
			}

		} while (bytesReceived > 0);

		closesocket(client);
		return;
	}
}

void TCPListener::Cleanup()
{
	WSACleanup();
}

SOCKET TCPListener::CreateSocket()
{
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening != INVALID_SOCKET)
	{
		sockaddr_in hint;

		hint.sin_family = AF_INET;
		hint.sin_port = htons(this->m_port);
		inet_pton(AF_INET, this->m_ipAddress.c_str(), &hint.sin_addr);

		int bindOk = bind(listening, (sockaddr*)&hint, sizeof(hint));

		if (bindOk != SOCKET_ERROR)
		{
			int listenOk = listen(listening, SOMAXCONN);

			if (listenOk == SOCKET_ERROR)
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}

	return listening;
}

SOCKET TCPListener::WaitForConnection(SOCKET listening)
{
	sockaddr_in s_client;
	int clientSize = sizeof(s_client);

	SOCKET client = accept(listening, (sockaddr*)&s_client, &clientSize);

	if (client == INVALID_SOCKET)
	{
		return -1;
	}

	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&s_client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << " Connected on port: " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &s_client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " Connected on port: " << ntohs(s_client.sin_port) << std::endl;
	}

	return client;
}

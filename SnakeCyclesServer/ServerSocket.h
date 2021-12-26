#pragma once

#include "Communication.h"
#include "ClientSocket.h"

class ServerSocket
{
public:
	ServerSocket();
	~ServerSocket();

	bool Listen(const char* ipAddr, int port);
	bool AcceptSockets();
	// send start message to client, as well as their colors
	bool StartGame(Board* board);
	bool EndGame();
	bool Send(SendCommand commandToSend);
	bool Recv(Board* board);

	SOCKET GetSocket();
	fd_set GetClients();
private:
	bool Bind(const char* ipAddr, int port);

private:
	// Is WSA started up?
	bool WSAInit;
	SOCKET m_socket;
	ClientSockets m_clientSockets;
	//fd_set m_clientSockets;
};

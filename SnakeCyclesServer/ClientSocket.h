#pragma once

#include "Communication.h"
#include "Screen.h"

class ClientSockets
{
public:
	ClientSockets();
	~ClientSockets();
	bool Send(SendCommand commandToSend);
	bool Send(SendCommand commandToSend, int index);
	bool Recv(Board* board);

	fd_set* GetSet();

private:
	
private:
	fd_set m_clientSockets;
};

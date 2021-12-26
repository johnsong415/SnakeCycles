#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <conio.h>
#include <thread>

#include "Types.h"

bool SendToClients(fd_set clientSockets, SendCommand commandToSend);
bool SendToClient(SOCKET clientSocket, SendCommand commandToSend);
SOCKET CreateSocket();
int Listen(SOCKET listeningSocket, const char* ipAddress, int port);
bool AcceptClientSockets(fd_set* clientSockets, fd_set* socketSet, SOCKET listeningSocket);

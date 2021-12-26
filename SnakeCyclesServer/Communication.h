#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <conio.h>
#include <thread>

#include "Types.h"

bool SendToClient(fd_set clientSockets, SendCommand commandToSend);
bool SendToClient(fd_set clientSockets, SendCommand commandToSend, int index);

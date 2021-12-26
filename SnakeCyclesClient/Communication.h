#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <conio.h>
#include <thread>

#include "Types.h"

bool SendToServer(SOCKET serverSocket, SendCommand commandToSend);
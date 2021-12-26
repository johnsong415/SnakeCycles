#include "Communication.h"

bool SendToServer(SOCKET serverSocket, SendCommand commandToSend)
{
    int bytesSent = send(serverSocket, reinterpret_cast<char*>(&commandToSend), sizeof(SendCommand), 0);
    while (bytesSent < sizeof(SendCommand)) {
        if (bytesSent == SOCKET_ERROR) {
            int errCode = WSAGetLastError();
            printf("Send failed - Error code: %d\n", errCode);
            closesocket(serverSocket);
            return false;
        }

        bytesSent += send(serverSocket, &reinterpret_cast<char*>(&commandToSend)[bytesSent], sizeof(SendCommand) - bytesSent, 0);
    }
	return true;
}

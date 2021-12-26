#include "Communication.h"

bool SendToClient(fd_set clientSockets, SendCommand commandToSend)
{
    for (unsigned int i = 0; i < (clientSockets.fd_count); ++i) {
        SendToClient(clientSockets, commandToSend, i);
    }
    return true;
}

bool SendToClient(fd_set clientSockets, SendCommand commandToSend, int index)
{
    int bytesSent = send(clientSockets.fd_array[index], reinterpret_cast<char*>(&commandToSend), sizeof(SendCommand), 0);
    while (bytesSent < sizeof(SendCommand)) {
        if (bytesSent == SOCKET_ERROR) {
            int errCode = WSAGetLastError();
            printf("Send failed - Error code: %d\n", errCode);
            closesocket(clientSockets.fd_array[index]);
            return false;
        }

        bytesSent += send(clientSockets.fd_array[index], &reinterpret_cast<char*>(&commandToSend)[bytesSent], sizeof(SendCommand) - bytesSent, 0);
    }

    return true;
}

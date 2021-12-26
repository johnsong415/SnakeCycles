#include "ClientSocket.h"

ClientSockets::ClientSockets()
{
	FD_ZERO(&m_clientSockets);
}

ClientSockets::~ClientSockets()
{
    for (unsigned int i = 0; i < m_clientSockets.fd_count; ++i) {
        closesocket(m_clientSockets.fd_array[i]);
    }
}

bool ClientSockets::Send(SendCommand commandToSend)
{
	SendToClients(m_clientSockets, commandToSend);
	return true;
}

bool ClientSockets::Send(SendCommand commandToSend, int index)
{
    SendToClient(m_clientSockets.fd_array[index], commandToSend);
    return true;
}

bool ClientSockets::Recv(Board* board)
{
    timeval timeToWait;
    timeToWait.tv_sec = 0;
    timeToWait.tv_usec = 10;

    fd_set socketsToRead = m_clientSockets;
    int socketsReady = select(0, (&socketsToRead), nullptr, nullptr, &timeToWait);
    if (socketsReady == SOCKET_ERROR) {
        int errCode = WSAGetLastError();
        printf("select failed - Error code: %d\n", errCode);
        return false;
    }
    if (socketsReady > 0) {
        for (unsigned int i = 0; i < m_clientSockets.fd_count; ++i) {
            if (FD_ISSET(m_clientSockets.fd_array[i], &socketsToRead) != 0) {
                RecvCommand receivedCommand;
                int bytesRead = recv(m_clientSockets.fd_array[i], reinterpret_cast<char*>(&receivedCommand), sizeof(RecvCommand), 0);
                if (bytesRead == SOCKET_ERROR) {
                    int errCode = WSAGetLastError();
                    printf("recv failed - Error code: %d\n", errCode);
                    break;
                }
                if (bytesRead == 0) {
                    printf("Connection closed");

                    closesocket(m_clientSockets.fd_array[i]);
                }
                while (bytesRead < sizeof(RecvCommand)) {
                    bytesRead += recv(m_clientSockets.fd_array[i], &(reinterpret_cast<char*>(&receivedCommand)[bytesRead]), sizeof(RecvCommand) - bytesRead, 0);
                }
                if (receivedCommand.recvedCommand == DIRECTIONCHANGE) {
                    board->ChangePlayerDirection(i, receivedCommand.newDirection);
                }
            }
        }
    }
}

fd_set* ClientSockets::GetSet()
{
    return &m_clientSockets;
}

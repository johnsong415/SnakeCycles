#include "Communication.h"

bool SendToClients(fd_set clientSockets, SendCommand commandToSend)
{
    for (unsigned int i = 0; i < (clientSockets.fd_count); ++i) {
        SendToClient(clientSockets.fd_array[i], commandToSend);
    }
    return true;
}

bool SendToClient(SOCKET clientSocket, SendCommand commandToSend)
{
    int bytesSent = send(clientSocket, reinterpret_cast<char*>(&commandToSend), sizeof(SendCommand), 0);
    while (bytesSent < sizeof(SendCommand)) {
        if (bytesSent == SOCKET_ERROR) {
            int errCode = WSAGetLastError();
            printf("Send failed - Error code: %d\n", errCode);
            closesocket(clientSocket);
            return false;
        }

        bytesSent += send(clientSocket, &reinterpret_cast<char*>(&commandToSend)[bytesSent], sizeof(SendCommand) - bytesSent, 0);
    }

    return true;
}

SOCKET CreateSocket()
{
    WSADATA winSockInfo;

    WORD version = MAKEWORD(2, 2);
    int initialized = WSAStartup(version, &winSockInfo);
    if (initialized != 0) {
        int errCode = WSAGetLastError();
        printf("WSA start up failed - Error code: %d\n", errCode);
        return 0;
    }

    SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listeningSocket == INVALID_SOCKET) {
        int errCode = WSAGetLastError();
        printf("Socket creation failed - Error code: %d\n", errCode);
        WSACleanup();
        return 0;
    }

    return listeningSocket;
}

int Listen(SOCKET listeningSocket, const char* ipAddress, int port)
{
    ULONG buffer;
    IN_ADDR transportAddress;
    inet_pton(AF_INET, ipAddress, &buffer);
    transportAddress.S_un.S_addr = buffer;

    sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = ntohs(port);
    socketAddress.sin_addr = transportAddress;
    int success = bind(listeningSocket, reinterpret_cast<sockaddr*>(&socketAddress), sizeof(socketAddress));
    if (success == SOCKET_ERROR) {
        int errCode = WSAGetLastError();
        printf("bind failed - Error code: %d\n", errCode);
        closesocket(listeningSocket);
        WSACleanup();
        return 0;
    }

    success = listen(listeningSocket, 20);
    if (success != 0) {
        int errCode = WSAGetLastError();
        printf("listen failed - Error code: %d\n", errCode);
        closesocket(listeningSocket);
        WSACleanup();
        return 0;
    }
    return success;
}

bool AcceptClientSockets(fd_set* clientSockets, fd_set* socketSet, SOCKET listeningSocket)
{
    timeval timeForClient;
    timeForClient.tv_sec = 1;
    timeForClient.tv_usec = 0;

    while (true) {
        if (clientSockets->fd_count < 5) {
            int socketsReady = select(0, socketSet, nullptr, nullptr, &timeForClient);
            if (socketsReady == SOCKET_ERROR) {
                int errCode = WSAGetLastError();
                printf("select failed - Error code: %d\n", errCode);
                closesocket(listeningSocket);
                WSACleanup();
                return false;
            }
            if (socketsReady == 1) {
                sockaddr_in clientSocketAddress;
                int addrLen = sizeof(sockaddr_in);
                SOCKET newSocket = accept(listeningSocket, reinterpret_cast<sockaddr*>(&clientSocketAddress), &addrLen);
                if (newSocket == INVALID_SOCKET) {
                    int errCode = WSAGetLastError();
                    printf("accept failed - Error code: %d\n", errCode);
                    closesocket(listeningSocket);
                    WSACleanup();
                    return false;
                }
                FD_SET(newSocket, clientSockets);
            }
        }
        // here the program will ask if # client sockets is all
        printf("There are %d client sockets connected. Start the game?\n[Y] for yes\n", clientSockets->fd_count);
        char response = _getch();
        if (tolower(response) == 'y') { break; }
    }
    return true;
}

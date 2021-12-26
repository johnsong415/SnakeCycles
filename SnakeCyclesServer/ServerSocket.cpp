#include "ServerSocket.h"

ServerSocket::ServerSocket()
{
    FD_ZERO(&m_clientSockets);
	//if (WSAInit == false) {
    WSADATA winSockInfo;

    WORD version = MAKEWORD(2, 2);
    int initialized = WSAStartup(version, &winSockInfo);
    if (initialized != 0) {
        int errCode = WSAGetLastError();
        printf("WSA start up failed - Error code: %d\n", errCode);
        return;
    }
    WSAInit = true;
	//}
    
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET) {
        int errCode = WSAGetLastError();
        printf("Socket creation failed - Error code: %d\n", errCode);
        WSACleanup();
        return;
    }
}

ServerSocket::~ServerSocket()
{
    m_clientSockets.~ClientSockets();
    closesocket(m_socket);
    WSACleanup();
}

bool ServerSocket::Bind(const char* ipAddr, int port)
{
    ULONG buffer;
    IN_ADDR transportAddress;
    inet_pton(AF_INET, ipAddr, &buffer);
    transportAddress.S_un.S_addr = buffer;

    sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = ntohs(port);
    socketAddress.sin_addr = transportAddress;
    int success = bind(m_socket, reinterpret_cast<sockaddr*>(&socketAddress), sizeof(socketAddress));
    if (success == SOCKET_ERROR) {
        int errCode = WSAGetLastError();
        printf("bind failed - Error code: %d\n", errCode);
        closesocket(m_socket);
        WSACleanup();
        return false;
    }
    return true;
}

bool ServerSocket::Listen(const char* ipAddr, int port)
{
    bool success = Bind(ipAddr, port);
    if (success == false) {
        return false;
    }

    int returned = listen(m_socket, 20);
    if (returned != 0) {
        int errCode = WSAGetLastError();
        printf("listen failed - Error code: %d\n", errCode);
        closesocket(m_socket);
        WSACleanup();
        return false;
    }

    return true;
}

bool ServerSocket::AcceptSockets()
{
    fd_set listeningSocket;
    FD_ZERO(&listeningSocket);
    FD_SET(m_socket, &listeningSocket);
    timeval timeForClient;
    timeForClient.tv_sec = 1;
    timeForClient.tv_usec = 0;

    while (true) {
        if ((m_clientSockets.GetSet())->fd_count < 5) {
            int socketsReady = select(0, &listeningSocket, nullptr, nullptr, &timeForClient);
            if (socketsReady == SOCKET_ERROR) {
                int errCode = WSAGetLastError();
                printf("select failed - Error code: %d\n", errCode);
                closesocket(m_socket);
                WSACleanup();
                return false;
            }
            if (socketsReady == 1) {
                sockaddr_in clientSocketAddress;
                int addrLen = sizeof(sockaddr_in);
                SOCKET newSocket = accept(m_socket, reinterpret_cast<sockaddr*>(&clientSocketAddress), &addrLen);
                if (newSocket == INVALID_SOCKET) {
                    int errCode = WSAGetLastError();
                    printf("accept failed - Error code: %d\n", errCode);
                    closesocket(m_socket);
                    WSACleanup();
                    return false;
                }
                FD_SET(newSocket, m_clientSockets.GetSet());
            }
        }
        // here the program will ask if # client sockets is all
        printf("There are %d client sockets connected. Start the game?\n[Y] for yes\n", m_clientSockets.GetSet()->fd_count);
        char response = _getch();
        if (response == 'Y') { break; }
    }
    return true;
}

bool ServerSocket::StartGame(Board* board)
{
    SendCommand startMessage;
    startMessage.sentCommand = STARTGAME;

    for (unsigned int i = 0; i < (m_clientSockets.GetSet())->fd_count; ++i) {
        if (i == 0) {
            board->AddPlayer(REDTEAM, (m_clientSockets.GetSet())->fd_array[i], i);
            startMessage.coordinates.team = REDTEAM;
        }
        else if (i == 1) {
            startMessage.coordinates.team = BLUETEAM;
            board->AddPlayer(BLUETEAM, m_clientSockets.GetSet()->fd_array[i], i);
        }
        else if (i == 2) {
            startMessage.coordinates.team = GREENTEAM;
            board->AddPlayer(GREENTEAM, m_clientSockets.GetSet()->fd_array[i], i);
        }
        else if (i == 3) {
            startMessage.coordinates.team = PURPLETEAM;
            board->AddPlayer(PURPLETEAM, m_clientSockets.GetSet()->fd_array[i], i);
        }
        m_clientSockets.Send(startMessage, i);
    }
    return true;
}

bool ServerSocket::EndGame()
{
    SendCommand endMessage;
    endMessage.sentCommand = ENDGAME;
    m_clientSockets.Send(endMessage);
    this->~ServerSocket();
    return true;
}

bool ServerSocket::Send(SendCommand commandToSend)
{
    return m_clientSockets.Send(commandToSend);;
}

bool ServerSocket::Recv(Board* board)
{
    return m_clientSockets.Recv(board);
}

SOCKET ServerSocket::GetSocket()
{
    return m_socket;
}

fd_set ServerSocket::GetClients()
{
    return *m_clientSockets.GetSet();
}

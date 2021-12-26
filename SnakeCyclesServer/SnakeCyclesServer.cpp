#include <iostream>

#include "Screen.h"
#include "Player.h"
#include "Communication.h"
#include <chrono>

int main()
{
    /* Communication stuff start */
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

    ULONG buffer;
    IN_ADDR transportAddress;
    inet_pton(AF_INET, "0.0.0.0", &buffer);
    transportAddress.S_un.S_addr = buffer;

    sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = ntohs(8989);
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
    /* Communication setup finished here */

    fd_set socketSet;
    FD_ZERO(&socketSet);
    FD_SET(listeningSocket, &socketSet);
    int socketsReady = select(0, &socketSet, nullptr, nullptr, NULL);
    if (socketsReady == SOCKET_ERROR) {
        int errCode = WSAGetLastError();
        printf("select failed - Error code: %d\n", errCode);
        closesocket(listeningSocket);
        WSACleanup();
        return 0;
    }
    
    timeval timeForClient;
    timeForClient.tv_sec = 1;
    timeForClient.tv_usec = 0;

    fd_set clientSockets;
    FD_ZERO(&clientSockets);
    while (true) {
        if (clientSockets.fd_count < 5) {
            int socketsReady = select(0, &socketSet, nullptr, nullptr, &timeForClient);
            if (socketsReady == SOCKET_ERROR) {
                int errCode = WSAGetLastError();
                printf("select failed - Error code: %d\n", errCode);
                closesocket(listeningSocket);
                WSACleanup();
                return 0;
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
                    return 0;
                }
                FD_SET(newSocket, &clientSockets);
            }
        }
        // here the program will ask if # client sockets is all
        printf("There are %d client sockets connected. Start the game?\n[Y] for yes\n", clientSockets.fd_count);
        char response = _getch();
        if (tolower(response) == 'y') { break; }
    }
    /* Secured connection */

    // send start message to client, as well as their colors
    SendCommand startMessage;
    startMessage.sentCommand = STARTGAME;

    Board* board = new Board(clientSockets);
    for (unsigned int i = 0; i < clientSockets.fd_count; ++i) {
        if (i == 0) {
            Player player = Player(REDTEAM);
            board->AddPlayer(reinterpret_cast<Player*>(&player), clientSockets.fd_array[i], i);
            startMessage.coordinates.team = REDTEAM;
            SendToClient(clientSockets, startMessage, i);
        }
        else if (i == 1) {
            Player player = Player(BLUETEAM);
            startMessage.coordinates.team = BLUETEAM;
            board->AddPlayer(reinterpret_cast<Player*>(&player), clientSockets.fd_array[i], i);
            SendToClient(clientSockets, startMessage, i);
        }
        else if (i == 2) {
            Player player = Player(GREENTEAM);
            startMessage.coordinates.team = GREENTEAM;
            board->AddPlayer(reinterpret_cast<Player*>(&player), clientSockets.fd_array[i], i);
            SendToClient(clientSockets, startMessage, i);
        }
        else if (i == 3) {
            Player player = Player(PURPLETEAM);
            startMessage.coordinates.team = PURPLETEAM;
            board->AddPlayer(reinterpret_cast<Player*>(&player), clientSockets.fd_array[i], i);
            SendToClient(clientSockets, startMessage, i);
        }
    }
    bool startGame = SendToClient(clientSockets, startMessage);
    if (!startGame) {
        printf("start game failed");
        closesocket(listeningSocket);
        WSACleanup();
        return 0;
    }

    std::chrono::steady_clock::time_point timePreviousPrint = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point timeStart = std::chrono::steady_clock::now();

    timeval timeToWait;
    timeToWait.tv_sec = 0;
    timeToWait.tv_usec = 10;

    while (true)
    {
        if (!board->GameValid()) { break; }

        fd_set socketsToRead = clientSockets;
        int socketsReady = select(0, (&socketsToRead), nullptr, nullptr, &timeToWait);
        if (socketsReady == SOCKET_ERROR) {
            int errCode = WSAGetLastError();
            printf("select failed - Error code: %d\n", errCode);
            break;
        }
        if (socketsReady > 0) {
            for (unsigned int i = 0; i < clientSockets.fd_count; ++i) {
                if (FD_ISSET(clientSockets.fd_array[i], &socketsToRead) != 0) {
                    RecvCommand receivedCommand;
                    int bytesRead = recv(clientSockets.fd_array[i], reinterpret_cast<char*>(&receivedCommand), sizeof(RecvCommand), 0);
                    if (bytesRead == SOCKET_ERROR) {
                        int errCode = WSAGetLastError();
                        printf("recv failed - Error code: %d\n", errCode);
                        break;
                    }
                    if (bytesRead == 0) {
                        printf("Connection closed");

                        closesocket(clientSockets.fd_array[i]);
                    }
                    while (bytesRead < sizeof(RecvCommand)) {
                        bytesRead += recv(clientSockets.fd_array[i], &(reinterpret_cast<char*>(&receivedCommand)[bytesRead]), sizeof(RecvCommand) - bytesRead, 0);
                    }
                    if (receivedCommand.recvedCommand == DIRECTIONCHANGE) {
                        board->ChangePlayerDirection(i, receivedCommand.newDirection);
                    }
                }
            }
        }
        std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration timeElapsed = timeNow - timePreviousPrint;
        std::chrono::milliseconds timeElapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(timeElapsed);
        // If 1/60th of a second passes
        if ((int)(timeElapsedMsec.count()) > (1000 / 60)) {
            board->Update();
            timePreviousPrint = timeNow;
        }
    }

    SendCommand endMessage;
    endMessage.sentCommand = ENDGAME;
    SendToClient(clientSockets, endMessage);
    closesocket(listeningSocket);
    for (unsigned int i = 0; i < clientSockets.fd_count; ++i) {
        closesocket(clientSockets.fd_array[i]);
    }
    WSACleanup();
    return 0;
}

#include <iostream>

#include "Screen.h"
#include "Player.h"
#include "ServerSocket.h"
#include <chrono>

int main()
{
    /* Communication stuff start */
    ServerSocket listeningSocket;
    bool listenSuccessful = listeningSocket.Listen("0.0.0.0", 8989);
    if (listenSuccessful) { return 0; }
    /* Communication setup finished here */
    //fd_set clientSockets;
    //FD_ZERO(&clientSockets);
    listeningSocket.AcceptSockets();
    /* Secured connection */

    Board* board = new Board(listeningSocket.GetClients());
    // send start message to client, as well as their colors
    listeningSocket.StartGame(board);
    
    std::chrono::steady_clock::time_point timePreviousPrint = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point timeStart = std::chrono::steady_clock::now();

    timeval timeToWait;
    timeToWait.tv_sec = 0;
    timeToWait.tv_usec = 10;

    while (true)
    {
        if (!board->GameValid()) { break; }

        listeningSocket.Recv(board);

        std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration timeElapsed = timeNow - timePreviousPrint;
        std::chrono::milliseconds timeElapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(timeElapsed);
        // If 1/60th of a second passes
        if ((int)(timeElapsedMsec.count()) > (1000 / 6)) {
            board->Update();
            timePreviousPrint = timeNow;
        }
    }
    listeningSocket.EndGame();

    return 0;
}

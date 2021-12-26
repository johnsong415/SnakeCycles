#pragma once

#include "Types.h"
#include "Player.h"
#include "Communication.h"

class Board
{
public:
	Board(fd_set sockets);
	~Board();
	void Update();
	void UpdatePlayer(Player* player);
	void KillPlayer(Player* player);
	//void AddPlayer(Player* player, SOCKET clientSocket, int index);
	void AddPlayer(Team playerTeam, SOCKET clientSocket, int index);
	void SetSockets(fd_set sockets);
	void ChangePlayerDirection(int index, Direction newDirection);
	bool GameValid();
private:
	struct PlayerAndSocket
	{
		Player* player = nullptr;
		SOCKET clientSocket = NULL;
	};
	// map
	Team m_map[500][500];
	// array of players
	PlayerAndSocket m_players[4];
	// actual set
	fd_set m_playerSet;
	//

};
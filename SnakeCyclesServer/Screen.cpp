#include "Screen.h"

Board::Board(fd_set sockets)
{
	m_playerSet = sockets;
	for (int x = 0; x < 500; ++x) {
		for (int y = 0; y < 500; ++y) {
			if (x == 0) { m_map[x][y] = BORDER; }
			else if (y == 0) { m_map[x][y] = BORDER; }
			else if (x == 499) { m_map[x][y] = BORDER; }
			else if (y == 499) { m_map[x][y] = BORDER; }
			else { m_map[x][y] = EMPTY; }
		}
	}
}

Board::~Board()
{
}

void Board::UpdatePlayer(Player* player)
{
	if (player->IsAlive() == false) { return; }
	Coordinates* head = player->GetHead();
	if (player->GetDirection() == UP) {
		if (head->y > 0) {
			head->y -= 1;
		}
	}
	else if (player->GetDirection() == DOWN) {
		if (head->y < 500) {
			head->y += 1;
		}
	}
	else if (player->GetDirection() == LEFT) {
		if (head->x > 0) {
			head->x -= 1;
		}
	}
	else if (player->GetDirection() == RIGHT) {
		if (head->x < 500) {
			head->x += 1;
		}
	}
	if (m_map[head->x][head->y] == EMPTY) {
		m_map[head->x][head->y] = player->GetTeam();
		SendCommand updateCommand;
		updateCommand.coordinates = *head;
		updateCommand.sentCommand = NEWLOC;
		SendToClient(m_playerSet, updateCommand);
	}
	else {
		KillPlayer(player);
	}
}

void Board::KillPlayer(Player* player)
{
	player->Die();
	SendCommand killCommand;
	killCommand.sentCommand = NEWLOC;
	killCommand.coordinates.team = EMPTY;
	for (int x = 0; x < 500; ++x) {
		for (int y = 0; y < 500; ++y) {
			if (m_map[x][y] == player->GetTeam()) {
				killCommand.coordinates.x = x;
				killCommand.coordinates.y = y;
				SendToClient(m_playerSet, killCommand);
				m_map[x][y] = EMPTY;
			}
		}
	}
}

void Board::AddPlayer(Player* player, SOCKET clientSocket, int index)
{
	PlayerAndSocket client;
	client.clientSocket = clientSocket;
	client.player = player;
	m_players[index] = client;
}

void Board::SetSockets(fd_set sockets)
{
	m_playerSet = sockets;
}

void Board::ChangePlayerDirection(int index, Direction newDirection)
{
	m_players[index].player->ChangeDirection(newDirection);
}

bool Board::GameValid()
{
	bool p1Alive = false;
	if (m_players[0].player != nullptr && m_players[0].player->IsAlive() == true) {
		p1Alive = true;
	}
	bool p2Alive = false;
	if (m_players[1].player != nullptr && m_players[1].player->IsAlive() == true) {
		p2Alive = true;
	}
	bool p3Alive = false;
	if (m_players[2].player != nullptr && m_players[2].player->IsAlive() == true) {
		p3Alive = true;
	}
	bool p4Alive = false;
	if (m_players[3].player != nullptr && m_players[3].player->IsAlive() == true) {
		p4Alive = true;
	}
	if (p1Alive || p2Alive || p3Alive || p4Alive) { return true; }
	else { return false; }
}

void Board::Update()
{
	if (m_players[0].player != nullptr) { UpdatePlayer(m_players[0].player); }
	if (m_players[1].player != nullptr) { UpdatePlayer(m_players[1].player); }
	if (m_players[2].player != nullptr) { UpdatePlayer(m_players[2].player); }
	if (m_players[3].player != nullptr) { UpdatePlayer(m_players[3].player); }
}

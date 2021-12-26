#include "Screen.h"

Board::Board()
{
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

void Board::Update(int x, int y, Team team)
{
	m_map[x][y] = team;
}

#pragma once

#include "Types.h"

class Player
{
public:
	Player(Team team);
	Direction GetDirection();
	void ChangeDirection(Direction newDirection);
	Coordinates* GetHead();
	void SetTeam(Team team);
	Team GetTeam();
	bool IsAlive();
	void Die();
private:
	Direction m_direction;
	Team m_team;
	Coordinates m_head;
	bool m_isAlive;
};
#include "Player.h"

Player::Player(Team team)
{
	m_team = team;
	m_isAlive = true;
	if (team == REDTEAM) {
		m_direction = UP;
		m_head.x = 249;
		m_head.y = 499;
		m_head.team = REDTEAM;
	}
	else if (team == BLUETEAM) {
		m_direction = LEFT;
		m_head.x = 499;
		m_head.y = 249;
		m_head.team = BLUETEAM;
	}
	else if (team == GREENTEAM) {
		m_direction = DOWN;
		m_head.x = 249;
		m_head.y = 0;
		m_head.team = GREENTEAM;
	}
	else {
		m_direction = RIGHT;
		m_head.x = 0;
		m_head.y = 249;
		m_head.team = PURPLETEAM;
	}
}

Direction Player::GetDirection()
{
	return m_direction;
}

void Player::ChangeDirection(Direction newDirection)
{
	if (newDirection == LEFT && m_direction != RIGHT) {
		m_direction = LEFT;
	}
	else if (newDirection == RIGHT && m_direction != LEFT) {
		m_direction = RIGHT;
	}
	else if (newDirection == UP && m_direction != DOWN) {
		m_direction = UP;
	}
	else if (newDirection == DOWN && m_direction != UP) {
		m_direction = DOWN;
	}
}

Coordinates* Player::GetHead()
{
	return &m_head;
}

void Player::SetTeam(Team team)
{
	m_team = team;
}

Team Player::GetTeam()
{
	return m_team;
}

bool Player::IsAlive()
{
	return m_isAlive;
}

void Player::Die()
{
	m_isAlive = false;
}

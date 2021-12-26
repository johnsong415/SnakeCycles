#pragma once

#include <Windows.h>

#include "Types.h"

class Screen
{
public:
	Screen(HDC hDC);
	void PrintScreen();
	void PrintBorders();
	void UpdatePoint(Coordinates coordinates);
	void DefineTeam(Team team);
public:
	static bool m_programValid;
private:
	HDC m_hDC;
	COLORREF m_background;
	COLORREF m_border;
	COLORREF m_redTeam;
	COLORREF m_blueTeam;
	COLORREF m_greenTeam;
	COLORREF m_purpleTeam;
	Team m_myTeam;
};

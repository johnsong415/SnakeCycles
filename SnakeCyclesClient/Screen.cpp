#include "Screen.h"

Screen::Screen(HDC hDC)
{
	m_hDC = hDC;
	m_background = RGB(255, 255, 255);
	m_border = RGB(0, 0, 0);
	m_redTeam = RGB(255, 0, 0);
	m_blueTeam = RGB(0, 0, 255);
	m_greenTeam = RGB(0, 255, 0);
	m_purpleTeam = RGB(150, 0, 150);
	m_myTeam = REDTEAM;
}

void Screen::PrintScreen()
{
	LOGBRUSH logBrush;
	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = m_background;
	HBRUSH brush = CreateBrushIndirect(&logBrush);
	RECT screenArea;
	screenArea.left = 50;
	screenArea.top = 50;
	screenArea.right = 550;
	screenArea.bottom = 550;
	FillRect(m_hDC, &screenArea, brush);
	for (int n = 0; n < 500; ++n) {
		SetPixel(m_hDC, n + 50, 50, m_border);
		SetPixel(m_hDC, n + 50, 550, m_border);
		SetPixel(m_hDC, 50, n + 50, m_border);
		SetPixel(m_hDC, 550, n + 50, m_border);
	}
}

void Screen::PrintBorders()
{
	// horizontal top
	MoveToEx(m_hDC, 50, 50, nullptr);
	LineTo(m_hDC, 549, 50);
	// vertical right
	LineTo(m_hDC, 549, 549);
	// vertical right
	LineTo(m_hDC, 50, 549);
	// horizontal bottom
	LineTo(m_hDC, 50, 50);
}

void Screen::UpdatePoint(Coordinates coordinates)
{
	COLORREF teamColor = m_redTeam;
	if (coordinates.team == BLUETEAM) { teamColor = m_blueTeam; }
	else if (coordinates.team == GREENTEAM) { teamColor = m_greenTeam; }
	else if (coordinates.team == PURPLETEAM) { teamColor = m_purpleTeam; }
	else if (coordinates.team == EMPTY) { teamColor = m_background; }

	SetPixel(m_hDC, coordinates.x + 50, coordinates.y + 50, teamColor);
}

void Screen::DefineTeam(Team team)
{
	m_myTeam = team;
}

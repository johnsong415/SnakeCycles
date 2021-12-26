#pragma once

enum CommandReceive
{
    NEWLOC = 0,
    STARTGAME,
    ENDGAME
};

enum CommandGive
{
    DIRECTIONCHANGE = 0,
};

enum Team
{
    REDTEAM = 0,
    BLUETEAM,
    GREENTEAM,
    PURPLETEAM,
    BORDER,
    EMPTY
};

enum Direction
{
    UP = 0,
    DOWN,
    LEFT,
    RIGHT
};

struct Coordinates
{
    int x;
    int y;
    Team team;
};

struct RecvCommand
{
    CommandReceive recvedCommand;
    Coordinates coordinates;
};

struct SendCommand
{
    CommandGive sendCommand;
    Direction newDirection;
};

struct InitInstanceRet
{
    BOOL initiated;
    HWND hWnd;
};

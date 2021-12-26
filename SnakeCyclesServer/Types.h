#pragma once

enum CommandGive
{
    NEWLOC = 0,
    STARTGAME,
    ENDGAME
};

enum CommandReceive
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

struct SendCommand
{
    CommandGive sentCommand;
    Coordinates coordinates;
};

struct RecvCommand
{
    CommandReceive recvedCommand;
    Direction newDirection;
};

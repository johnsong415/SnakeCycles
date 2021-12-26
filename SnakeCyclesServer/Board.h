#include "Types.h"

class Board
{
public:
	Board();
	~Board();
	void Update(int x, int y, Team team);

private:
	Team m_map[500][500];
};
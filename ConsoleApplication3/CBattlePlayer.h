#pragma once
#include "CAquatory.h"
#include "CShip.h"
#include <vector>
#include <fstream>
#include <set>

#include "winsock2.h" 

class CBattlePlayer
{
public:
	CBattlePlayer();
	~CBattlePlayer();

	CAquatory m_Aqua;
	vector <CShip*> m_Ships;
	set <pair<char, int>> m_Moves;

	SOCKET m_sock;
	bool PrepareShips();
	bool ReadFromFile(string fileRecieve[10]);
	bool ShipsAreReady();
	bool DoMove();

	bool ComputerDoMove();
	bool ComputerPrepareShips();

	bool Try2PlaceShip(string ship);
	bool Try2DoMove(string str);
	void Message(string str);

	string recieve();
	string recieveFile();

	bool Try2RandomAquatory();

	CBattlePlayer *m_pAnotherPlayer;

	bool IsAlive();
};


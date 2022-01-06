#include "CBattlePlayer.h"

#define TEST
CBattlePlayer::CBattlePlayer()
{
	m_sock = 0;
	m_pAnotherPlayer = NULL;

}
CBattlePlayer::~CBattlePlayer()
{
	for (size_t i = 0; i < m_Ships.size(); i++)
	{
		delete m_Ships[i];
	}

	m_Ships.clear();

}
void CBattlePlayer::Message(string str)
{
	str = "\n" + str + "\n";
	send(m_sock, str.c_str(), str.size() + 1, 0);
}

bool CBattlePlayer::PrepareShips()
{
	Message("Расставляйте корабли!");
	
	while (!ShipsAreReady())
	{
		if (Try2PlaceShip(recieve()))
		{
			Message("OK");
		}
		else
		{
			Message("Ошибка в расположении корабля!");
		}
		Message(m_Aqua.PrintForeign());
	}
	Message("Ваши корабли готовы!");
	
	return true;
}
bool CBattlePlayer::DoMove()
{
	Message("Ваш ход!");
	string move = recieve();

	CShip *ship=NULL;
	if (m_pAnotherPlayer->m_Aqua.TestShip(move, &ship))
	{
		Message("Попадание!");

		if (!ship->Alive())
		{
			Message("Корабль потоплен!");

			if (!m_pAnotherPlayer->IsAlive())
			{
				Message("Вы выиграли(");
				m_pAnotherPlayer->Message("Вы проиграли(");
				return true;
			}
		}
		
		DoMove();

	}
	else
		Message("Мимо(");
	
	return true;
}

bool CBattlePlayer::ShipsAreReady()
{
#ifndef TEST
	if (m_Ships.size() != 10)
		return false;

#endif
	int n4Deck, n3Deck, n2Deck, n1Deck;

	n4Deck = n3Deck = n2Deck = n1Deck = 0;
	
	for (size_t i = 0; i < m_Ships.size(); i++)
	{
		if (!m_Ships[i]->IsPlaced())
			return false;
		else {
			switch (m_Ships[i]->m_Deck)
			{
			case 1:
				n1Deck++;
				break;
			case 2:
				n2Deck++;
				break;
			case 3:
				n3Deck++;
				break;
			case 4:
				n4Deck++;
				break;

			}
		}
	}

#ifdef TEST
	if (n4Deck == 1)// временно
#else
	if (n1Deck == 4 and n2Deck == 3 and n3Deck == 2 and n4Deck == 1)
#endif	
		return true;

	return false;
}
bool CBattlePlayer::Try2PlaceShip(string ship)
{
	int iDeck = 0;
	char buf[50];
	sscanf_s(ship.c_str(), "%i(%[^)]", &iDeck,buf,50);
	
	char cell[4][10];

	if (iDeck == 4)
	{
		sscanf_s(buf, "%[^,],%[^,],%[^,],%[^\n],", cell[0],10,cell[1],10, cell[2], 10, cell[3], 10);
	}
	if (iDeck == 3)
	{
		sscanf_s(buf, "%[^,],%[^,],%[^\n],", cell[0], 10, cell[1], 10, cell[2], 10);
	}
	if (iDeck == 2)
	{
		sscanf_s(buf, "%[^,],%[^\n],", cell[0], 10, cell[1], 10);
	}
	if (iDeck == 1)
	{
		sscanf_s(buf, "%[^\n],", cell[0], 10);
	}
	
	CShip *s = new CShip(iDeck);

	for (int i = 0; i < iDeck; i++)
	{
		CCell *pCell= NULL;
		if (m_Aqua.Try2PlaceDeck(cell[i],&pCell,s))
		{
			s->m_pCells.push_back(pCell);
			pCell->m_pShip = s;
		}
		else
		{
			s->ClearCells();
			delete s;
			return false;
		}
	}

	m_Ships.push_back(s);

	return true;
}
string CBattlePlayer::recieve()
{
	char buff[1024];

	recv(m_sock, &buff[0], sizeof(buff), 0);

	return buff;
}
bool CBattlePlayer::IsAlive()
{
	for (size_t i = 0; i < m_Ships.size(); i++)
	{
		if (m_Ships[i]->Alive())
			return true;
	}

	return false;
}
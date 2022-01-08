#include "CBattlePlayer.h"

#define TEST = 0
#include <iostream>
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
	str = str + "\n";
	//char buf[10];
	send(m_sock, str.c_str(), str.size() + 1, 0);
	Sleep(10);
	//recv(m_sock, &buf[0], 1, 0);
}

bool CBattlePlayer::ReadFromFile(string filepath, string fileRecieve[10])
{
	ifstream file(filepath, ios_base::in);
	if (!file.is_open())
		return false;
	string temp;
	for (int i = 0; i != 10; ++i)
	{
		if (!(file >> fileRecieve[i]))
		{
			file.close();
			return false;
		}
	}
	file.close();

	return true;
}

bool CBattlePlayer::PrepareShips()
{
	//ofstream debug("..\\data\\debug.txt");
	string buf; // Разработка метода задания расположения кораблей игроком из заранее подготовленного файла
	char mode;
	Message("Расставить корабли из заранее подготовленного файла? Y/N");
	while (true)
	{
		do
		{		
			buf = recieve();
			sscanf_s(buf.c_str(), "%c", &mode, 1);
			mode = toupper(mode);
			if (mode != 'N' && mode != 'Y')
				Message("Неверный ввод");
			else
				break;
		} while (true);
		if (mode == 'N')
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
				//Message(m_Aqua.PrintForeign());
			}
			Message("Ваши корабли готовы!");
		}
		else
		{	
			string fileRecieve[10];
			if (!ReadFromFile("..\\data\\ships.txt", fileRecieve))
			{
				Message("Ошибка чтения с файла\nРасставить корабли из заранее подготовленного файла? Y/N");
				continue;
			}
			//Message("Идёт чтение данных из файла");
			for (int i = 0; i != 10; ++i)
			{
				if (!Try2PlaceShip(fileRecieve[i]))
				{
					Message("Ошибка в расположении корабля!");
				}
				// debug << fileRecieve[i] << std::endl;
				//Message(m_Aqua.PrintForeign());
			}
			//recieve();
			//Message(m_Aqua.PrintForeign());
			//Message(to_string(ShipsAreReady()));
			//Message("Ваши корабли готовы!");
			//debug.close();
		}
		return true;
	}
}
bool CBattlePlayer::DoMove()
{
	Message("Ваш ход!");
	string move;
	do
	{
		move = recieve();
		if (Try2DoMove(move))
			break;
		Message("Неверный ввод");
	} while (!Try2DoMove(move));

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
	// Проверка правильности ввода
	if (iDeck <= 0 || buf[0] == -52 || buf[1] == -52)
		return false;

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
bool CBattlePlayer::Try2DoMove(string str)
{
	if (str.size() > 3 || str.size() == 0)
		return false;
	char letter;
	int number;
	sscanf_s(str.c_str(), "%c%i", &letter, 1, &number);
	letter = tolower(letter);
	if (letter < 97 || letter > 106 || number <= 0 || number > 10)
		return false;

	return true;
}
string CBattlePlayer::recieve()
{
	char buff[1024];
	Message("command_input");
	int nsize = recv(m_sock, &buff[0], sizeof(buff), 0);
	Sleep(10);

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
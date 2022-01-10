#include "CBattlePlayer.h"

#define TEST = 0
#include <iostream>
#include <ctime>
CBattlePlayer::CBattlePlayer()
{
	m_sock = 0;
	m_pAnotherPlayer = NULL;
	m_Moves = {};
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

bool CBattlePlayer::ReadFromFile(string fileRecieve[10])
{
	string fileInput = recieveFile();
	if (fileInput == "NULL" || count(fileInput.begin(), fileInput.end(), '\n') != 10)
		return false;
	int pos = 0;
	for (int i = 0;i != 10; ++i)
	{
		pos = fileInput.find('\n');
		fileRecieve[i] = fileInput.substr(0, pos);
		fileInput.erase(0, pos + 1);
	}

	return true;
}

bool CBattlePlayer::Try2RandomAquatory()
{
	int shipsToPlace[5] = { 0, 4, 3, 2, 1 };
	srand(time(0));
	int x, y;
	char letter;
	bool isXGreater;
	bool isXLess;
	bool isYGreater;
	bool isYLess;
	string shipString = "";
	for (int i = 4; i != 1; --i)
	{
		while (shipsToPlace[i])
		{
			x = rand() % 10;
			isXGreater = (x + i - 1) > 9;
			isXLess = (x - i + 1) < 0;
			if (isXGreater && isXLess)
				continue;
			y = rand() % 10;
			isYGreater = (y + i - 1) > 9;
			isYLess = (y - i + 1) < 0;
			if (isYGreater && isYLess)
				continue;
			letter = 'A' + y;
			shipString = to_string(i) + "(";
			if (isXGreater)
			{
				for (int j = 0; j != i; ++j)
				{
					shipString += string(1, letter) + to_string(x - j + 1) + ",";
				}
				if (Try2PlaceShip(shipString))
				{
					shipsToPlace[i] -= 1;
					continue;
				}
			}
			shipString = to_string(i) + "(";
			if (isXLess)
			{
				for (int j = 0; j != i; ++j)
				{
					shipString += string(1, letter) + to_string(x + j + 1) + ",";
				}
				if (Try2PlaceShip(shipString))
				{
					shipsToPlace[i] -= 1;
					continue;
				}
			}
			shipString = to_string(i) + "(";
			if (isYGreater)
			{
				for (int j = 0; j != i; ++j)
				{
					shipString += string(1, letter - j) + to_string(x + 1) + ",";
				}
				if (Try2PlaceShip(shipString))
				{
					shipsToPlace[i] -= 1;
					continue;
				}
			}
			shipString = to_string(i) + "(";
			if (isYLess)
			{
				for (int j = 0; j != i; ++j)
				{
					shipString += string(1, letter + j) + to_string(x + 1) + ",";
				}
				if (Try2PlaceShip(shipString))
				{
					shipsToPlace[i] -= 1;
					continue;
				}
			}
		}
	}
	while (shipsToPlace[1])
	{
		x = rand() % 10;
		y = rand() % 10;
		letter = 'A' + y;
		shipString = "1(" + string(1, letter) + to_string(x + 1);
		if (Try2PlaceShip(shipString))
			shipsToPlace[1] -= 1;
	}

	return true;
}

bool CBattlePlayer::ComputerPrepareShips()
{
	while (!ShipsAreReady())
	{
		Try2RandomAquatory();
		Message(m_Aqua.PrintOwn());
	}

	return true;
}

bool CBattlePlayer::PrepareShips()
{
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
			Message("Расставить корабли случайно? Y/N");
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
			if (mode == 'Y')
			{
				while (!ShipsAreReady())
				{
					Try2RandomAquatory();
					Message(m_Aqua.PrintOwn());
				}
			}
			else
			{
				while (!ShipsAreReady())
				{
					if (Try2PlaceShip(recieve()))
					{
						Message("OK");
						Message(m_Aqua.PrintOwn());
					}
					else
					{
						Message("Ошибка в расположении корабля!");
					}				
				}
				Message("Ваши корабли готовы! Ожидайте соперника");
			}
		}
		else
		{	
			string fileRecieve[10];
			if (!ReadFromFile(fileRecieve))
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
			}
			Message(m_Aqua.PrintOwn());
			//recieve();
			//Message(m_Aqua.PrintForeign());
			//Message(to_string(ShipsAreReady()));
			//Message("Ваши корабли готовы!");
			//debug.close();
		}
		Message("Ваши корабли готовы! Ожидайте соперника");
		return true;
	}
}

bool CBattlePlayer::ComputerDoMove()
{
	srand(time(0));
	int x, y;
	char letter;
	string move = "";

	do
	{
		x = rand() % 10;
		y = rand() % 10;
		letter = 'A' + y;
		move = string(1, letter) + to_string(x + 1);
		if (Try2DoMove(move))
			break;
		Sleep(50);
	} while (true);
	

	CShip* ship = NULL;
	if (m_pAnotherPlayer->m_Aqua.TestShip(move, &ship))
	{
		m_pAnotherPlayer->Message("Соперник  попал на " + move);

		if (!ship->Alive())
		{
			if (!m_pAnotherPlayer->IsAlive())
			{
				m_pAnotherPlayer->Message("Вы проиграли(");
				return true;
			}
			else
			{
				m_pAnotherPlayer->Message("Ваш корабль потоплен(\n");
			}
		}
		m_pAnotherPlayer->Message(m_pAnotherPlayer->m_Aqua.PrintOwn());
		Sleep(1000);
		ComputerDoMove();
	}
	else
	{
		m_pAnotherPlayer->Message("Противник промахнулся) по " + move + "\n" + m_pAnotherPlayer->m_Aqua.PrintOwn());
	}
	Sleep(1000);

	return true;
}

bool CBattlePlayer::DoMove()
{
	//Message(m_Aqua.PrintOwn());
	Message("Ваш ход!\n" + m_pAnotherPlayer->m_Aqua.PrintForeign());
	string move = "";
	do
	{
		move = recieve();
		if (Try2DoMove(move))
			break;
		Message("Неверный ввод");
	} while (true);

	CShip *ship=NULL;
	if (m_pAnotherPlayer->m_Aqua.TestShip(move, &ship))
	{
		Message("Попадание!");
		m_pAnotherPlayer->Message("Соперник  попал на " + move);

		if (!ship->Alive())
		{
			Message("Корабль потоплен!");

			if (!m_pAnotherPlayer->IsAlive())
			{
				Message("Вы выиграли)");
				m_pAnotherPlayer->Message("Вы проиграли(");
				return true;
			}
			else
			{
				m_pAnotherPlayer->Message("Ваш корабль потоплен(\n");
			}
		}
		m_pAnotherPlayer->Message(m_pAnotherPlayer->m_Aqua.PrintOwn());
		DoMove();
	}
	else
	{
		Message("Мимо(\n");
		m_pAnotherPlayer->Message("Противник промахнулся) по " + move + "\n" + m_pAnotherPlayer->m_Aqua.PrintOwn());
	}
	
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
	// Проверка правильности ввода
	if (iDeck <= 0 || strlen(buf) < 2 || strlen(buf) >= 60)
		return false;
	bool rowEqual = true;
	bool columnEqual = true;
	for (int i = 1; i != iDeck; ++i)
	{
		if (cell[i - 1][0] != cell[i][0])
			columnEqual = false;
		if (cell[i - 1][1] != cell[i][1])
			rowEqual = false;
	}
	if (rowEqual + columnEqual == 0)
		return false;
	CShip* s = new CShip(iDeck);

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
	int num, number;
	if (!m_Aqua.ParseCell(str, num, number))// Try2DoMove(move))
		return false;
	if (m_pAnotherPlayer->m_Aqua.m_Cells[num][number].m_bBeated)
		return false;
	//if (str.size() > 3 || str.size() == 0)
	//	return false;
	//char letter;
	//int number;
	//sscanf_s(str.c_str(), "%c%i", &letter, 1, &number);
	//letter = tolower(letter);
	//if (letter < 97 || letter > 106 || number <= 0 || number > 10)
	//	return false;
	//if (m_pAnotherPlayer->m_Aqua.m_Cells[letter - 97][number - 1].m_bBeated)
	//	return false;
	//pair<char, int> tempPair = make_pair(letter, number);
	//if (m_Moves.count(tempPair) > 0)
	//	return false;	
	//m_Moves.insert(make_pair(letter, number));

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
string CBattlePlayer::recieveFile()
{
	char buff[1024];
	Message("command_file");
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
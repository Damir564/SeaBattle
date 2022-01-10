#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "CBattleGame.h"

#pragma comment (lib, "ws2_32.lib")
#include <iostream>

#include "winsock2.h" 

using namespace std;

CBattleGame::CBattleGame()
{
	m_iCurrentMove = MOVE1;
	m_Player1.m_pAnotherPlayer = &m_Player2;
	m_Player2.m_pAnotherPlayer = &m_Player1;
}
CBattleGame::~CBattleGame()
{

}

bool CBattleGame::StartTCP(int port)
{
	setlocale(LC_ALL, "Rus");
	
	WSADATA wsaData;
	
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("Error WSAStartup %i\n", WSAGetLastError());
		return false;
	}
	
	SOCKET mysocket;
	
	if ((mysocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		printf("Error socket %i\n", WSAGetLastError());
		WSACleanup(); // �������������� ���������� Winsock
		return false;
	}
	
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(port); 
	local_addr.sin_addr.s_addr = 0; 
									
	if (bind(mysocket, (sockaddr *)&local_addr, sizeof(local_addr)))
	{
		
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(mysocket); 
		WSACleanup();
		return false;
	}
	
	if (listen(mysocket, 0x100))
	{
		printf("Error listen %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return false;
	}
	printf("�������� ����\n");
	
	SOCKET client_socket; 
	sockaddr_in client_addr;
 
	int client_addr_size = sizeof(client_addr);
	
	
		client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size);
		
		
		m_Player1.m_sock = client_socket;

		HOSTENT *hst;
		hst = gethostbyaddr((char *)&client_addr.sin_addr.s_addr, 4, AF_INET);

		// ����� �������� � �������
		printf("�����1 � IP [%s] �� �����!\n", inet_ntoa(client_addr.sin_addr));
		m_Player1.Message("������� ������ ����������? Y/N");
		string buf; // ���������� ������ ������� ������������ �������� ������� �� ������� ��������������� �����
		char mode;
		do
		{
			buf = m_Player1.recieve();
			sscanf_s(buf.c_str(), "%c", &mode, 1);
			mode = toupper(mode);
			if (mode != 'N' && mode != 'Y')
				m_Player1.Message("�������� ����");
			else
				break;
		} while (true);
		if (mode == 'Y')
		{
			m_playWithComputer = true;
			return true;
		}
		m_playWithComputer = false;
		client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size);

		m_Player2.m_sock = client_socket;
				
		hst = gethostbyaddr((char *)&client_addr.sin_addr.s_addr, 4, AF_INET);

		// ����� �������� � �������
		printf("�����2 � IP [%s] �� �����!\n", inet_ntoa(client_addr.sin_addr));
		
	return true;
}

void CBattleGame::prepareShips()
{
	m_Player1.PrepareShips();
	m_Player2.PrepareShips();
}

void CBattleGame::DoPlay()
{
	thread player1Thread(&CBattlePlayer::PrepareShips, &m_Player1);
	thread player2Thread;
	if (!m_playWithComputer)
		player2Thread = thread(&CBattlePlayer::PrepareShips, &m_Player2);
	else
		player2Thread = thread(&CBattlePlayer::ComputerPrepareShips, &m_Player2);
	player2Thread.join();
	player1Thread.join();
	m_Player2.Message("��� ���������");
	//m_Player2.Message("�������� ����������� �������. ��������� ��������� �����.");

	//if (m_Player1.PrepareShips())
	//{
	//	m_Player1.Message("�������� ����������� �������. ��������� ��������� �����.");

	//}
	//if (!m_Player2.PrepareShips())
	//{
	//	return;
	//}

	while (m_Player1.IsAlive() && m_Player2.IsAlive())
	{
		if (m_iCurrentMove == MOVE1)
			m_Player1.DoMove();
		
		if (m_iCurrentMove == MOVE2)
		{
			if (!m_playWithComputer)
				m_Player2.DoMove();
			else
				m_Player2.ComputerDoMove();
		}
		
		m_iCurrentMove *= -1;
	}
	m_Player1.Message("command_quit");
	m_Player2.Message("command_quit");
}
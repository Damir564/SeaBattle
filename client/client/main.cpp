#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib, "ws2_32.lib")

// ������ �������� TCP-�������
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <locale>
#include <iostream>

#define PORT 10000
#define SERVERADDR "127.0.0.1"
int main()
{
  setlocale(LC_ALL, "RUS");

  WSAData wsaData;
  char buff[1024];
  printf("������ �� �������!\n");
  // ��� 1 - ������������� ���������� Winsock
  if (WSAStartup(MAKEWORD(2,2), (WSADATA *)&wsaData))
  {
    printf("WSAStart ������ %i\n", WSAGetLastError());
    return -1;
  }
  // ��� 2 - �������� ������
  SOCKET my_sock;
  my_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (my_sock<0)
  {
    printf("Socket() ������ %i\n", WSAGetLastError());
    return -1;
  }
  // ��� 3 - ��������� ����������
  // ���������� ��������� sockaddr_in � �������� ������ � ����� �������
  sockaddr_in dest_addr;
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(PORT);
  HOSTENT *hst;
  // �������������� IP ������ �� ����������� � ������� ������
  if (inet_addr(SERVERADDR) != INADDR_NONE)
    dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
  else
    // ������� �������� IP ����� �� ��������� ����� �������
    if (hst = gethostbyname(SERVERADDR))
      // hst->h_addr_list �������� �� ������ �������,
      // � ������ ���������� �� ������
      ((unsigned long *)&dest_addr.sin_addr)[0] =
      ((unsigned long **)hst->h_addr_list)[0][0];
    else
    {
      printf("������� ����� %s\n", SERVERADDR);
      closesocket(my_sock);
      WSACleanup();
      return -1;
    }
    // ����� ������� ������� � �������� ���������� ����������
    if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
    {
      printf("������� ����� %i\n", WSAGetLastError());
      return -1;
    }
  printf("���������� � �������� %s ������� �����������\n\
         �������� quit ��� ����������", SERVERADDR);
  // ��� 4 - ������ � �������� ���������
  int nsize;
  while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR)
  {
    // ������ ����������� ���� � ����� ������
    buff[nsize] = 0;
    // ������� �� �����
    printf("%s", buff);
    // ������ ���������������� ���� � ����������
    printf("����: "); fgets(&buff[0], sizeof(buff) - 1, stdin);
    // �������� �� "quit"
    if (!strcmp(&buff[0], "quit\n"))
    {
      // ���������� �����
      printf("�����...");
      closesocket(my_sock);
      WSACleanup();
      return 0;
    }
    // �������� ������ ������� �������
    send(my_sock, &buff[0], nsize, 0);
  }
  printf("Recv error %d\n", WSAGetLastError());
  closesocket(my_sock);
  WSACleanup();
  return -1;
}
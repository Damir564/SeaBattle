#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib, "ws2_32.lib")

// Пример простого TCP-клиента
#include <stdio.h>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <locale>
#include <iostream>
#include <fstream>

#define PORT 10000
#define SERVERADDR "127.0.0.1"

using namespace std;


string readFromFile(string filepath)
{
    ifstream file(filepath, ios_base::in);
    if (!file.is_open())
        return "NULL";
    string answer = "";
    string temp;
    for (int i = 0; i != 10; ++i)
    {
        if (!(file >> temp))
        {
            file.close();
            return "NULL";
        }
        answer += temp + "\n";
    }
    file.close();

    return answer;
}

int main()
{
  setlocale(LC_ALL, "RUS");

  WSAData wsaData;
  char buff[1024];
  printf("Игра морской бой!\n");
  // Шаг 1 - инициализация библиотеки Winsock
  if (WSAStartup(MAKEWORD(2,2), (WSADATA *)&wsaData))
  {
    printf("WSAStart ошибка %i\n", WSAGetLastError());
    return -1;
  }
  // Шаг 2 - создание сокета
  SOCKET my_sock;
  my_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (my_sock<0)
  {
    printf("Socket() ошибка %i\n", WSAGetLastError());
    return -1;
  }
  // Шаг 3 - установка соединения
  // заполнение структуры sockaddr_in – указание адреса и порта сервера
  sockaddr_in dest_addr;
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(PORT);
  HOSTENT *hst;
  // преобразование IP адреса из символьного в сетевой формат
  if (inet_addr(SERVERADDR) != INADDR_NONE)
    dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
  else
    // попытка получить IP адрес по доменному имени сервера
    if (hst = gethostbyname(SERVERADDR))
      // hst->h_addr_list содержит не массив адресов,
      // а массив указателей на адреса
      ((unsigned long *)&dest_addr.sin_addr)[0] =
      ((unsigned long **)hst->h_addr_list)[0][0];
    else
    {
      printf("Инвалид адрес %s\n", SERVERADDR);
      closesocket(my_sock);
      WSACleanup();
      return -1;
    }
    // адрес сервера получен – пытаемся установить соединение
    if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
    {
      printf("Коннект эррор %i\n", WSAGetLastError());
      return -1;
    }
  printf("Соединение с сервером %s успешно установлено\n\
         Наберите command_quit для завершения\n", SERVERADDR);
  // Шаг 4 - чтение и передача сообщений
  int nsize;
  string userInput;
  while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR)
  {
    // ставим завершающий ноль в конце строки
    buff[nsize] = 0;
    // выводим на экран
    // читаем пользовательский ввод с клавиатуры
    if (!strcmp(&buff[0], "command_input\n"))
    {
        printf("Ввод: ");
        getline(std::cin, userInput);
        send(my_sock, userInput.c_str(), userInput.size() + 1, 0);
        Sleep(10);
    }
    else if (!strcmp(&buff[0], "command_file\n"))
    {
        userInput = readFromFile("..\\data\\ships.txt");
        send(my_sock, userInput.c_str(), userInput.size() + 1, 0);
    }
    else if (!strcmp(&buff[0], "command_quit\n"))
    {
        // Корректный выход
        printf("Экзит...");
        closesocket(my_sock);
        WSACleanup();
        system("pause");
        return 0;
    }
    else
    {
        printf("%s", buff);
        Sleep(10);
    }
    // проверка на "quit"
    
    // передаем строку клиента серверу
  }
  printf("Recv error %d\n", WSAGetLastError());
  closesocket(my_sock);
  WSACleanup();
  return -1;
}
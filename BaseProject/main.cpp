#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

void ModClConfigScreen()
{
	WSADATA wsaData;
	
	string IP;
	unsigned int port;

	cout << "Enter Modbus server IP address: ";
	cin >> IP;
	cout << "Enter Modbus server socket. no: ";
	cin >> port;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0)
	{
		cout << "WSAStartup failed. Error: " << result << endl;
		return -1;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	InetPton(AF_INET, IP, &server.sin_addr);
}

void ModServer(){
	cout << "Modbus server" << endl << endl;
}

void ModClient(){
	int sel;

	cout << "Modbus client" << endl << endl;
	cout << "1. Configuration" << endl;
	cout << "2. Connect" << endl;
	cout << "3. Back" << endl;

	cin >> sel;

	switch(sel){
		case 1:
			system("cls");
			ModClConfigScreen();
			break;
		case 2:
			system("cls");
			break;
		case 3:
			system("cls");
			startScreen();
			break;
		default:
			system("cls");
			ModClient();
	}
}

void Selection()
{
	int option;

	cin >> option;

	switch(option){
		case 1:
			system("cls");
			ModServer();
			break;
		case 2:
			system("cls");
			ModClient();
			break;
		case 3:
			system("cls");
			cout << "Exit" << endl;
			break;
		default:
			system("cls");
			cout << "Select your modbus application" << endl << endl;
			cout << "1. Server" << endl;
			cout << "2. Client" << endl;
			cout << "3. Exit" << endl << endl;
			cout<<"Invalid option. Enter a valid option."<<endl;
			Selection();
	}
}

void startScreen(void) {

	system("cls");
	cout << "Select your modbus application" << endl << endl;
	cout << "1. Server" << endl;
	cout << "2. Client" << endl;
	cout << "3. Exit" << endl << endl;

	Selection();
}

int main() {
	startScreen();
	WSACleanup();
	std::cin.get();
	return 0;
}

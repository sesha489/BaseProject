#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <stdio.h>

#pragma once;
#pragma comment(lib, "Ws2_32.lib")

enum Screen
{
	START,
	CLIENT,
	CLIENT_SETTING,
	CLIENT_OPERATION,
	SERVER,
	SERVER_SETTING,
	SERVER_OPERATION,
	EXIT
};

struct clientDetails {
	std::string IP = "192.168.1.10";
	unsigned int deviceID = 1;
	unsigned int port = 502;

	bool connected = false;
};

struct serverDetails {
	std::string IP = "192.168.1.10";
	unsigned int deviceID = 1;
	unsigned int port = 502;

	bool connected = false;
};

Screen StartScreen(void);
Screen ModClient(void);
Screen ModCliSetting(clientDetails& Client);
void EditCliSetting(clientDetails& Client);
Screen ModCliOper(clientDetails& Client);
void CliOper(clientDetails& Client);
Screen ModServer(void);
Screen ModSerSetting(void);
Screen ModSerOper(void);

Screen StartScreen(void) {
	int option;

	system("cls");
	std::cout << "Select your modbus application" << std::endl << std::endl;
	std::cout << "1. Server" << std::endl;
	std::cout << "2. Client" << std::endl;
	std::cout << "3. Exit" << std::endl << std::endl;

	std::cin >> option;

	switch (option) {
	case 1:
		return SERVER;
	case 2:
		return CLIENT;
	case 3:
		return EXIT;
	}

	return START;
}

Screen ModClient(void) {
	int sel;

	system("cls");
	std::cout << "Modbus client" << std::endl << std::endl;
	std::cout << "1. Setting" << std::endl;
	std::cout << "2. Connect" << std::endl;
	std::cout << "3. Back" << std::endl << std::endl;

	std::cin >> sel;

	switch (sel) {
	case 1:
		return CLIENT_SETTING;
	case 2:
		return CLIENT_OPERATION;
	case 3:
		return START;
	}

	return CLIENT;
}

Screen ModCliSetting(clientDetails& Client)
{
	int sel;

	system("cls");
	std::cout << "Enter Modbus client IP address: " << Client.IP << std::endl;
	std::cout << "Enter Modbus client device ID. no: " << Client.deviceID << std::endl;
	std::cout << "Enter Modbus client socket. no: " << Client.port << std::endl << std::endl;

	std::cout << "1. Edit setting" << std::endl;
	std::cout << "2. Connect" << std::endl;
	std::cout << "3. Back" << std::endl << std::endl;

	std::cin >> sel;

	switch (sel) {
	case 1:
		EditCliSetting(Client);
		return CLIENT_SETTING;
	case 2:
		return CLIENT_OPERATION;
	case 3:
		return CLIENT;
	}

	return CLIENT_SETTING;
}

void EditCliSetting(clientDetails& Client)
{
	std::cout << "Enter Modbus client IP address: ";
	std::cin >> Client.IP;
	std::cout << "Enter Modbus client device ID. no: ";
	std::cin >> Client.deviceID;
	std::cout << "Enter Modbus client socket. no: ";
	std::cin >> Client.port;
}

Screen ModCliOper(clientDetails& Client)
{
	int sel;

	system("cls");
	std::cout << "1. Connect" << std::endl;
	std::cout << "2. Back" << std::endl;

	std::cin >> sel;

	switch (sel) {
	case 1:
		CliOper(Client);
		return CLIENT_OPERATION;
	case 2:
		return CLIENT;
	}

	return CLIENT_OPERATION;
}

void CliOper(clientDetails& Client) {
	system("cls");

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		std::cout << "Socket creation failed. Error: " << WSAGetLastError() << std::endl;
		Client.connected = false;
		return;
	}
	//std::cout << "Socket created" << std::endl;

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(Client.port);
	if (InetPtonA(AF_INET, Client.IP.c_str(), &server.sin_addr) != 1) {
		std::cout << "Invalid IP address format." << std::endl;
		Client.connected = false;
		closesocket(sock);
		return;
	}
	//std::cout << "IP address assigned" << std::endl;

	std::cout << "Connecting " << Client.IP << ":" << Client.port << "..." << std::endl;
	int result = connect(sock, reinterpret_cast<sockaddr*>(& server), sizeof(server));
	if (result == SOCKET_ERROR) {
		std::cout << "Connection failed. Error: " << WSAGetLastError() << std::endl;
		Client.connected = false;
		closesocket(sock);
		return;
	}
	std::cout << "Connected" << std::endl << std::endl;
	std::cout << "Polling every 1000ms" << std::endl << std::endl;
	Client.connected = true;

	uint8_t txBuffer[] =
	{
		0x00,0x01,     // Transaction ID

		0x00,0x00,     // Protocol ID

		0x00,0x06,     // Length

		0x01,          // Unit ID

		0x03,          // Function Code

		0x00,0x63,     // Starting Address

		0x00,0x02      // Quantity
	};
	uint8_t rxBuffer[25];
	int packetSent = 0;
	int packetReceived = 0;

	while (Client.connected) {
		int bytesSent = send(sock, reinterpret_cast<const char*>(txBuffer), sizeof(txBuffer), 0);

		if (bytesSent == SOCKET_ERROR)
		{
			std::cout << WSAGetLastError();
			Client.connected = false;
			closesocket(sock);
			return;
		}
		else
		{
			std::cout << bytesSent << " bytes sent" << std::endl;
			packetSent++;
		}

		int bytesReceived = recv(sock, reinterpret_cast<char*>(rxBuffer), sizeof(rxBuffer), 0);
		if (bytesReceived == SOCKET_ERROR || bytesReceived == 0)
		{
			if (bytesReceived == 0)
				std::cout << "Server disconnected." << std::endl;
			else
				std::cout << "Receive failed. Error: " << WSAGetLastError() << std::endl;
			Client.connected = false;
			closesocket(sock);
			return;
		}
		else
		{
			std::cout << bytesReceived << " bytes received" << std::endl;
			packetReceived++;
		}

		std::cout << std::endl << "Transmitted bytes" << std::endl;
		for (int i = 0; i < sizeof(txBuffer); i++)
		{
			printf("%02X ", txBuffer[i]);
		}
		printf("\n");
		std::cout << "Received bytes" << std::endl;
		for (int i = 0; i < bytesReceived; i++)
		{
			printf("%02X ", rxBuffer[i]);
		}
		printf("\n\n");

		std::cout << "Packets Sent: " << packetSent << std::endl;
		std::cout << "Packets Received: " << packetReceived << std::endl;
		std::cout << "Status : Connected" << std::endl;

		Sleep(1000);
		system("cls");
		std::cout << "Connecting " << Client.IP << ":" << Client.port << "..." << std::endl;
		std::cout << "Connected" << std::endl << std::endl;
		std::cout << "Polling every 1000ms" << std::endl << std::endl;
	}
}

Screen ModServer(void){
	int sel;

	system("cls");
	std::cout << "Modbus server" << std::endl << std::endl;
	std::cout << "1. Setting" << std::endl;
	std::cout << "2. Connect" << std::endl;
	std::cout << "3. Back" << std::endl;

	std::cin >> sel;

	switch (sel) {
	case 1:
		return SERVER_SETTING;
	case 2:
		return SERVER_OPERATION;
	case 3:
		return START;
	}

	return SERVER;
}

Screen ModSerSetting(void) {
	return SERVER;
}

Screen ModSerOper(void) {
	return SERVER;
}

int main() {
	clientDetails client;
	serverDetails server;
	Screen current = START;

	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		std::cout << "WSAStartup failed. Error: " << result << std::endl;
		return 0;
	}
	std::cout << "Winsock initialized successfully!" << std::endl << std::endl;

	while (current != EXIT && result == 0)
	{
		switch (current) {
		case START:
			current = StartScreen();
			break;
		case CLIENT:
			current = ModClient();
			break;
		case CLIENT_SETTING:
			current = ModCliSetting(client);
			break;
		case CLIENT_OPERATION:
			current = ModCliOper(client);
			break;
		case SERVER:
			current = ModServer();
			break;
		case SERVER_SETTING:
			current = ModSerSetting();
			break;
		case SERVER_OPERATION:
			current = ModSerOper();
			break;
		}
	}
	std::cout << "Exiting" << std::endl;

	WSACleanup();

	std::cin.get();
	return 0;
}

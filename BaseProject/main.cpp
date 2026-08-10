#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

//#pragma once ---- This is required only in H file not belongs to CPP
#pragma comment(lib, "Ws2_32.lib")

std::atomic<bool> running(true);
std::atomic<bool> pollingPaused(false);

enum Screen
{
	START,
	CLIENT,
	CLIENT_OPERATION,
	SERVER,
	SERVER_OPERATION,
	EXIT
};

struct clientDetails {
	std::string IP = "192.168.1.10";
	unsigned int deviceID = 1;
	unsigned int port = 502;

	unsigned int functionCode = 3;
	unsigned int startingAddress = 0;
	unsigned int quantity = 2;

	SOCKET sock = INVALID_SOCKET;
	bool connected = false;
};

struct serverDetails {
	std::string IP = "192.168.1.10";
	unsigned int deviceID = 1;
	unsigned int port = 502;

	bool connected = false;
};

Screen StartScreen(void);
void ClientHeader(clientDetails& Client);
Screen ModClient(clientDetails& Client);
void EditCliSetting(clientDetails& Client);
void EditCliParaSetting(clientDetails& Client);
Screen ModCliOper(clientDetails& Client);
void disconnectPoll(clientDetails& Client);
std::vector<uint8_t> buildRequest(const clientDetails& Client);
void CliOper(clientDetails& Client);
Screen ModServer(void);
Screen ModSerOper(void);

Screen StartScreen(void) {
	int option;

	system("cls");
	std::cout << "============================================================" << std::endl;
	std::cout << "                         MODBUS Tool                        " << std::endl;
	std::cout << "============================================================" << std::endl << std::endl;
	std::cout << "1. Modbus Server" << std::endl;
	std::cout << "2. Modbus Client" << std::endl;
	std::cout << "3. Exit" << std::endl << std::endl;
	std::cout << "Select: ";
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

void ClientHeader(clientDetails& Client) {
	system("cls");
	std::cout << "============================================================" << std::endl;
	std::cout << "                       MODBUS Client                        " << std::endl;
	std::cout << "============================================================" << std::endl;
	std::cout << "Connection details" << std::endl;
	std::cout << "IP: " << Client.IP << "     Port: " << Client.port << std::endl;
	std::cout << "Status: " << (Client.connected ? "Connected" : "Disconnected") << std::endl;
	std::cout << "------------------------------------------------------------" << std::endl;
	std::cout << "Parameters" << std::endl;
	std::cout << "Device ID: " << Client.deviceID << "     Function code: " << Client.functionCode << std::endl;
	std::cout << "Starting address: " << Client.startingAddress << "     Quantity: " << Client.quantity << std::endl;
	std::cout << "------------------------------------------------------------" << std::endl;
	std::cout << "Polling" << std::endl;
	std::cout << "Polling every 1000ms          Total Count: 0" << std::endl;
	std::cout << "Success: 0     Failure: 0     Exception: 0" << std::endl;
	std::cout << "------------------------------------------------------------" << std::endl;
}

Screen ModClient(clientDetails& Client) {
	int sel;

	ClientHeader(Client);
	std::cout << "Menu" << std::endl;
	std::cout << "1.Edit Connection  2.Edit Parameters  3.Debug Packet  4.Exit" << std::endl;
	std::cout << "5." << (Client.connected ? "Disconnect" : "Connect") << std::endl;
	std::cout << "------------------------------------------------------------" << std::endl;
	std::cout << "Select: ";
	std::cin >> sel;

	switch (sel) {
	case 1:
		EditCliSetting(Client);
		return CLIENT;
	case 2:
		EditCliParaSetting(Client);
		return CLIENT;
	case 4:
		return START;
	case 5:
		return CLIENT_OPERATION;
	}

	return CLIENT;
}

void EditCliSetting(clientDetails& Client)
{
	ClientHeader(Client);
	std::cout << "Enter Modbus client IP address   : ";
	std::cin >> Client.IP;
	std::cout << "Enter Modbus client device ID. no: ";
	std::cin >> Client.deviceID;
	std::cout << "Enter Modbus client port         : ";
	std::cin >> Client.port;
}

void EditCliParaSetting(clientDetails& Client)
{
	ClientHeader(Client);
	std::cout << "Enter Modbus client device ID. no   : ";
	std::cin >> Client.deviceID;
	std::cout << "Enter Modbus client function code   : ";
	std::cin >> Client.functionCode;
	std::cout << "Enter Modbus client starting address: ";
	std::cin >> Client.startingAddress;
	std::cout << "Enter Modbus client quantity        : ";
	std::cin >> Client.quantity;
}

Screen ModCliOper(clientDetails& Client)
{
	int sel;
	running = true;

	std::thread t1(CliOper, std::ref(Client));
	std::thread t2(disconnectPoll, std::ref(Client));
	t1.join();
	t2.join();
	ClientHeader(Client);
	std::cout << "Menu" << std::endl;
	std::cout << "1.Edit Parameters  2.Debug Packet" << std::endl;
	std::cout << "3." << (Client.connected ? "Disconnect" : "Connect") << std::endl;
	std::cout << "------------------------------------------------------------" << std::endl;

	if (Client.connected) {
		return CLIENT_OPERATION;
	}
	else {
		return CLIENT;
	}
}

void disconnectPoll(clientDetails& Client) {
	int sel;
	while (running) {
		std::cin >> sel;
		if (sel == 3) {
			std::cout << "Disconnecting..." << std::endl;
			Client.connected = false;
			closesocket(Client.sock);
			Client.sock = INVALID_SOCKET;
			running = false;
			return;
		}
		if (sel == 1) {
			pollingPaused = true;
			EditCliParaSetting(Client);
			pollingPaused = false;
		}
	}
}

std::vector<uint8_t> buildRequest(const clientDetails& Client)
{
	uint16_t transactionID = 1;

	std::vector<uint8_t> txBuffer =
	{
		static_cast<uint8_t>(transactionID >> 8),
		static_cast<uint8_t>(transactionID & 0xFF),

		0x00,
		0x00,

		0x00,
		0x06,

		static_cast<uint8_t>(Client.deviceID),

		static_cast<uint8_t>(Client.functionCode),

		static_cast<uint8_t>(Client.startingAddress >> 8),
		static_cast<uint8_t>(Client.startingAddress & 0xFF),

		static_cast<uint8_t>(Client.quantity >> 8),
		static_cast<uint8_t>(Client.quantity & 0xFF)
	};

	return txBuffer;
}

void CliOper(clientDetails& Client) {

	Client.sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Client.sock == INVALID_SOCKET) {
		std::cout << "Socket creation failed. Error: " << WSAGetLastError() << std::endl;
		std::cout << "Press 3 to go back" << std::endl;
		Client.connected = false;
		Client.sock = INVALID_SOCKET;
		running = false;
		return;
	}
	//std::cout << "Socket created" << std::endl;

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(Client.port);
	if (InetPtonA(AF_INET, Client.IP.c_str(), &server.sin_addr) != 1) {
		std::cout << "Invalid IP address format." << std::endl;
		std::cout << "Press 3 to go back" << std::endl;
		Client.connected = false;
		closesocket(Client.sock);
		Client.sock = INVALID_SOCKET;
		running = false;
		return;
	}
	//std::cout << "IP address assigned" << std::endl;

	std::cout << "Connecting " << Client.IP << ":" << Client.port << "..." << std::endl;
	int result = connect(Client.sock, reinterpret_cast<sockaddr*>(&server), sizeof(server));
	if (result == SOCKET_ERROR) {
		std::cout << "Connection failed. Error: " << WSAGetLastError() << std::endl;
		std::cout << "Press 3 to go back" << std::endl;
		Client.connected = false;
		closesocket(Client.sock);
		Client.sock = INVALID_SOCKET;
		running = false;
		return;
	}
	Client.connected = true;

	ClientHeader(Client);
	std::cout << "Menu" << std::endl;
	std::cout << "1.Edit Parameters  2.Debug Packet" << std::endl;
	std::cout << "3." << (Client.connected ? "Disconnect" : "Connect") << std::endl;
	std::cout << "------------------------------------------------------------" << std::endl;

	std::vector<uint8_t> txBuffer = buildRequest(Client);
	uint8_t rxBuffer[25];
	int packetSent = 0;
	int packetReceived = 0;

	while (Client.connected && running) {
		while (pollingPaused)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		txBuffer = buildRequest(Client);
		int bytesSent = send(Client.sock, reinterpret_cast<const char*>(txBuffer.data()), txBuffer.size(), 0);

		if (bytesSent == SOCKET_ERROR)
		{
			std::cout << WSAGetLastError();
			std::cout << "Press 3 to go back" << std::endl;
			Client.connected = false;
			closesocket(Client.sock);
			Client.sock = INVALID_SOCKET;
			running = false;
			return;
		}
		else
		{
			std::cout << bytesSent << " bytes sent" << std::endl;
			packetSent++;
		}

		int bytesReceived = recv(Client.sock, reinterpret_cast<char*>(rxBuffer), sizeof(rxBuffer), 0);
		if (bytesReceived == SOCKET_ERROR || bytesReceived == 0)
		{
			if (bytesReceived == 0)
				std::cout << "Server disconnected." << std::endl;
			else {
				std::cout << "Receive failed. Error: " << WSAGetLastError() << std::endl;
				std::cout << "Press 3 to go back" << std::endl;
				Client.connected = false;
				closesocket(Client.sock);
				Client.sock = INVALID_SOCKET;
				running = false;
				return;
			}
		}
		else
		{
			std::cout << bytesReceived << " bytes received" << std::endl;
			packetReceived++;
		}

		std::cout << std::endl << "Transmitted bytes" << std::endl;
		for (int i = 0; i < txBuffer.size(); i++)
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

		//Sleep(1000);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "POLLING RESUMED\n";  //-----------------------------Print overwrite issue check
		ClientHeader(Client);
		std::cout << "Menu" << std::endl;
		std::cout << "1.Edit Parameters  2.Debug Packet" << std::endl;
		std::cout << "3." << (Client.connected ? "Disconnect" : "Connect") << std::endl;
		std::cout << "------------------------------------------------------------" << std::endl;
	}
	Client.connected = false;
	closesocket(Client.sock);
	Client.sock = INVALID_SOCKET;
	running = false;
	return;
}

Screen ModServer(void) {
	int sel;

	system("cls");
	std::cout << "Modbus server" << std::endl << std::endl;
	std::cout << "1. Setting" << std::endl;
	std::cout << "2. Connect" << std::endl;
	std::cout << "3. Back" << std::endl;

	std::cin >> sel;

	switch (sel) {
	case 1:
		return SERVER;
	case 2:
		return SERVER_OPERATION;
	case 3:
		return START;
	}

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
			current = ModClient(client);
			break;
		case CLIENT_OPERATION:
			current = ModCliOper(client);
			break;
		case SERVER:
			current = ModServer();
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

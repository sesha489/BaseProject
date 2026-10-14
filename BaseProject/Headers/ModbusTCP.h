#pragma once				//This is required only in H file not belongs to CPP

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <iostream>
#include <stdio.h>

#include <vector>

class ModbusTCP {
public:
	std::vector<uint8_t> rxBuffer;

	ModbusTCP();
	~ModbusTCP();
};
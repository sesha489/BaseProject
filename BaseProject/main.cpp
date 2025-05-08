#include<iostream>

using namespace std;

void startScreen(void) {
	int option;
	option = 0;

	cout << "Select your modbus application" << endl;
	cout << "1. Client" << endl;
	cout << "2. Server" << endl;
	cout << "3. Exit" << endl;
	cin >> option;
}

int main() {
	startScreen();
	return 0;
}

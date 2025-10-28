#include<iostream>

using namespace std;

struct IP {
	unsigned int ip1;
	unsigned int ip2;
	unsigned int ip3;
	unsigned int ip4;
};

void ModClConfigScreen()
{
	IP MasterIP;
	IP SubnetIP;
	unsigned int port;
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
	std::cin.get();
	return 0;
}

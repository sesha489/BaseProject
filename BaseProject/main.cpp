#include<iostream>

using namespace std;

void ModServer(){
	cout << "Modbus server" << endl;
}

void ModClient(){
	cout << "Modbus client" << endl;
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
			cout << "Select your modbus application" << endl;
			cout << "1. Server" << endl;
			cout << "2. Client" << endl;
			cout << "3. Exit" << endl;
			cout<<"Invalid option. Enter a valid option."<<endl;
			Selection();
	}
}

void startScreen(void) {

	system("cls");
	cout << "Select your modbus application" << endl;
	cout << "1. Server" << endl;
	cout << "2. Client" << endl;
	cout << "3. Exit" << endl;

	Selection();
}

int main() {
	startScreen();
	std::cin.get();
	return 0;
}

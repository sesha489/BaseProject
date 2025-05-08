#include<iostream>

using namespace std;

void ModServer(){
	cout << "Modbus server" << endl;
}

void ModClient(){
	cout << "Modbus client" << endl;
}

void Selection(int optionSel)
{
	switch(optionSel){
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
			cout<<"Not a valid option. Enter a valid option."<<endl;
	}
}

void startScreen(void) {
	int option;
	option = 0;

	system("cls");
	cout << "Select your modbus application" << endl;
	cout << "1. Server" << endl;
	cout << "2. Client" << endl;
	cout << "3. Exit" << endl;
	cin >> option;

	Selection(option);
}

int main() {
	startScreen();
	std::cin.get();
	return 0;
}

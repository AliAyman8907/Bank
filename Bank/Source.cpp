#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<iomanip>
#include<algorithm>
#include<cctype>
#include"MyLib.h";

using namespace std;

//Name of the file where all client information will be stored
const string ClientsFileName = "Clients.txt";

//Enum that represents the main operations the program can perform
enum enDoWhat{ShowClientList = 1, AddNewClient = 2, DeleteClient = 3,
			  UpdateClientInfo = 4, FindClient = 5 , Exit = 6};

//Enum that maps each client field
enum enClientInfo{AccountNum, PinCode, Name, Phone, Balance};

//Represents a single client's data
struct stClientInfo {
	string AccountNum;
	short PinCode;
	string name;
	string Phone;
	int balance;
};

void MainMenuScreen();

//Convert a client record into a line string to save in the file
string convertRecordToLine(const stClientInfo& clientInfo, const string& sep = "#//#") {
	return clientInfo.AccountNum + sep +
		to_string(clientInfo.PinCode) + sep +
		clientInfo.name + sep +
		clientInfo.Phone + sep +
		to_string(clientInfo.balance);
}

//Convert a line string from the file into a client record
stClientInfo convertLineToRecord(const string& line, const string& sep = "#//#") {
	vector<string> parts = MyLib::splitString(line, sep);
	stClientInfo clientInfo{};

	clientInfo.AccountNum = parts[enClientInfo::AccountNum];
	clientInfo.PinCode = static_cast<short>(stoi(parts[enClientInfo::PinCode]));
	clientInfo.name = parts[enClientInfo::Name];
	clientInfo.Phone = parts[enClientInfo::Phone];
	clientInfo.balance = stoi(parts[enClientInfo::Balance]);

	return clientInfo;
}

//Load all clients' info from the file into a vector
vector<stClientInfo> loadClientInfoFromFile() {
	string line;
	vector<stClientInfo> vClientsInfo;

	fstream ClientsInfo;
	ClientsInfo.open(ClientsFileName, ios::in);
	if (ClientsInfo.is_open()) {
		while (getline(ClientsInfo, line)) {
			vClientsInfo.push_back(convertLineToRecord(line));
		}
	}
	ClientsInfo.close();

	return vClientsInfo;
}

//Save all clients' info from a vector into the file
void saveClientsInfoToFile(const vector<stClientInfo>& vClientsInfo) {
	fstream ClientsInfo;
	ClientsInfo.open(ClientsFileName, ios::out);
	if (ClientsInfo.is_open()) {
		for (const stClientInfo& line : vClientsInfo) {
			ClientsInfo << convertRecordToLine(line) << '\n';
		}
	}
	ClientsInfo.close();
}

//Print details of a single client
void printClientInfo(const vector<stClientInfo>& vClientsInfo, short vClientsInfoIndex) {
	cout << "\nThe following are the client details:\n";
	cout << "-----------------------------------------\n";
	cout << "Account Number: " << vClientsInfo[vClientsInfoIndex].AccountNum << '\n';
	cout << "Pin Code      : " << vClientsInfo[vClientsInfoIndex].PinCode << '\n';
	cout << "Name          : " << vClientsInfo[vClientsInfoIndex].name << '\n';
	cout << "Phone         : " << vClientsInfo[vClientsInfoIndex].Phone << '\n';
	cout << "Balance       : " << vClientsInfo[vClientsInfoIndex].balance << '\n';
	cout << "-----------------------------------------\n";
}

//Read client info from user
void enterClientInfo(stClientInfo& clientInfo) {
	cout << "Enter Pin Code: ";
	cin >> clientInfo.PinCode;
	while (cin.fail())
	{
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "Invalid number, Enter a valid one : ";
		cin >> clientInfo.PinCode;
	}

	cout << "Enter Name: ";
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(cin, clientInfo.name);

	cout << "Enter Phone: ";
	cin >> clientInfo.Phone;
	while (!all_of(clientInfo.Phone.begin(), clientInfo.Phone.end(), ::isdigit))
	{
		cout << "Invalid number, Enter a valid one: ";
		cin >> clientInfo.Phone;
	}

	cout << "Enter Balance: ";
	cin >> clientInfo.balance;
	while (cin.fail())
	{
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "Invalid number, Enter a valid one : ";
		cin >> clientInfo.balance;
	}
}

//Find client index in the vector by account number
short findClientIndexByAccountNum(const vector<stClientInfo>& vClientsInfo, const string& accountNum) {
	for (int i = 0; i < vClientsInfo.size(); i++) {
		string currentAccountNum = vClientsInfo[i].AccountNum;
		if (currentAccountNum == accountNum) {
			return i;
		}
	}
	return -1;
}

//Read Yes/No input from user
char readYesNo(const string& message) {
	char answer;
	cout << message;
	cin >> answer;
	while (toupper(answer) != 'Y' && toupper(answer) != 'N') {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Invalid input, please enter Y or N: ";
		cin >> answer;
	}

	return toupper(answer);
}

//Return to the main menu after finishing an operation
void goBackToMainMenu() {
	cout << "\n\nPress any key to go back to Main Menu...";
	system("pause>0");
	MainMenuScreen();
}

//Print the header of the client list screen
void printClientListScreenHeader(const vector <stClientInfo>& vClientsInfo) {
	string title = "Clients List [";
	title += to_string(vClientsInfo.size());
	title += "] Client(s).";

	MyLib::printCentered(title); 
	cout << '\n';
	MyLib::fillLineWithHypens();

	cout << left << setw(25) << "| Account Number";
	cout << left << setw(20) << "| Pin Code";
	cout << left << setw(45) << "| Client Name";
	cout << left << setw(20) << "| Phone";
	cout << "| Balance\n";
	MyLib::fillLineWithHypens();
}

//Show all clients on the screen in a table format
void showClientsScreen(vector<stClientInfo>& vClientsInfo) {
	printClientListScreenHeader(vClientsInfo);
	cout << '\n';

	for (int i = 0; i < vClientsInfo.size(); i++)
	{
		cout << "| "; cout << left << setw(23) << vClientsInfo[i].AccountNum;
		cout << "| "; cout << left << setw(18) << vClientsInfo[i].PinCode;
		cout << "| "; cout << left << setw(43) << vClientsInfo[i].name;
		cout << "| "; cout << left << setw(18) << vClientsInfo[i].Phone;
		cout << "| "; cout << vClientsInfo[i].balance << '\n';
	}
	cout << '\n';
	MyLib::fillLineWithHypens();
}

//Add a new client and save it to the file
void addNewClientScreen(vector<stClientInfo>& vClientsInfo, stClientInfo& clientInfo) {
	char addAnotherClient;
	short index;

	cout << "-----------------------------------------\n";
	cout << "          Add New Client Screen\n";
	cout << "-----------------------------------------\n";
	cout << "Adding New Client:\n";
	
	do
	{
		cout << "\n\nEnter Account Number: ";
		cin >> clientInfo.AccountNum;

		index = findClientIndexByAccountNum(vClientsInfo, clientInfo.AccountNum);
		while (index != -1) {
			cout << "\nClient with [" << clientInfo.AccountNum << "] already exists! , Enter another Account Number: ";
			cin >> clientInfo.AccountNum;
			index = findClientIndexByAccountNum(vClientsInfo, clientInfo.AccountNum);
		}

		enterClientInfo(clientInfo);

		vClientsInfo.push_back(clientInfo);

		fstream ClientsInfo;
		ClientsInfo.open(ClientsFileName, ios::out | ios::app);
		if (ClientsInfo.is_open())
		{
			ClientsInfo << convertRecordToLine(vClientsInfo[vClientsInfo.size() - 1]) << '\n';
		}
		ClientsInfo.close();

		addAnotherClient = readYesNo("\n\nClient added successfully. Do you want to add another client? Y/N? ");
	} while (toupper(addAnotherClient) == 'Y');
}

//Delete a client by account number
void deleteClientScreen(vector<stClientInfo>& vClientsInfo) {
	string accountNum;
	short index;
	char doOrDoNotDeleteClient;
	char deleteAnotherClient;

	cout << "-----------------------------------------\n";
	cout << "           Delete Client Screen\n";
	cout << "-----------------------------------------\n";

	do
	{
		cout << "\n\nPlease enter Account Number: ";
		cin >> accountNum;

		index = findClientIndexByAccountNum(vClientsInfo, accountNum);
		while (index == -1) {
			cout << "Client with Account Number [" << accountNum
				<< "] is not found!, Enter an Existing Account Number: ";
			cin >> accountNum;
			index = findClientIndexByAccountNum(vClientsInfo, accountNum);
		}

		printClientInfo(vClientsInfo, index);

		cout << "\n\nAre you sure you want to delete this client? Y/N? ";
		cin >> doOrDoNotDeleteClient;
		while (toupper(doOrDoNotDeleteClient) != 'Y' && toupper(doOrDoNotDeleteClient) != 'N') {
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			cout << "Invalid input, please enter Y or N: ";
			cin >> doOrDoNotDeleteClient;
		}

		if (toupper(doOrDoNotDeleteClient) == 'Y') {
			vClientsInfo.erase(vClientsInfo.begin() + index);
			saveClientsInfoToFile(vClientsInfo);

			deleteAnotherClient = readYesNo("\n\nClient Deleted Successfully. Do you want to delete another client? Y/N? ");
		}
	} while (toupper(deleteAnotherClient) == 'Y');
}

//Update client info (keeping the same account number)
void updateClientInfo(stClientInfo& clientInfo, string accountNum) {
	clientInfo.AccountNum = accountNum;
	enterClientInfo(clientInfo);
}

//Update a client's info and save the changes to the file
void updateClientInfoScreen(vector <stClientInfo>& vClientsInfo, stClientInfo& clientInfo) {
	string accountNum;
	short index;
	char doOrDoNotUpdateInfo;
	char updateAnotherClientInfo;

	cout << "-----------------------------------------\n";
	cout << "        Update Client Info Screen\n";
	cout << "-----------------------------------------\n";

	do
	{
		cout << "\nPlease enter Account Number: ";
		cin >> accountNum;

		index = findClientIndexByAccountNum(vClientsInfo, accountNum);
		while (index == -1) {
			cout << "Client with Account Number [" << accountNum
				<< "] is not found!, Enter an Existing Account Number: ";
			cin >> accountNum;
			index = findClientIndexByAccountNum(vClientsInfo, accountNum);
		}

		printClientInfo(vClientsInfo, index);

		cout << "\n\nAre you sure you want to update this client? Y/N? ";
		cin >> doOrDoNotUpdateInfo;
		while (toupper(doOrDoNotUpdateInfo) != 'Y' && toupper(doOrDoNotUpdateInfo) != 'N') {
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			cout << "Invalid input, please enter Y or N: ";
			cin >> doOrDoNotUpdateInfo;
		}

		if (toupper(doOrDoNotUpdateInfo) == 'Y') {
			cout << "\n\n";
			updateClientInfo(clientInfo, accountNum);
			vClientsInfo[index] = clientInfo;
			saveClientsInfoToFile(vClientsInfo);

			updateAnotherClientInfo = readYesNo("\n\nClient Updated Successfully. Do you want to update another client? Y/N? ");
		}
	} while (toupper(updateAnotherClientInfo) == 'Y');
}

//Find and display client details by account number
void findClientScreen(const vector <stClientInfo>& vClientsInfo) {
	string accountNum;
	short index;
	char findAnotherClient;

	cout << "-----------------------------------------\n";
	cout << "            Find Client Screen\n";
	cout << "-----------------------------------------\n";

	do
	{
		cout << "\nPlease enter Account Number: ";
		cin >> accountNum;

		index = findClientIndexByAccountNum(vClientsInfo, accountNum);
		while (index == -1) {
			cout << "Client with Account Number [" << accountNum
				<< "] is not found!, Enter an Existing Account Number: ";
			cin >> accountNum;
			index = findClientIndexByAccountNum(vClientsInfo, accountNum);
		}

		printClientInfo(vClientsInfo, index);

		findAnotherClient = readYesNo("\n\nDo you want to see another client details? Y/N? ");
	} while (toupper(findAnotherClient) == 'Y');
}

//Exit program
void exitScreen() {
	system("cls");
	cout << "-----------------------------------------\n";
	cout << "             Program Ends :-)\n";
	cout << "-----------------------------------------\n";
	system("pause>0");
}

//Read user choice (1–6) from the menu
enDoWhat readNumThatShowWhatToDo() {
	short doWhat;
	cout << "Choose what do you want to do? [1 to 6]? ";
	cin >> doWhat;
	while (cin.fail() || doWhat < 1 || doWhat > 6) {
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "Invalid number, Enter 1, 2, 3, 4, 5 or 6: ";
		cin >> doWhat;
	}

	return static_cast<enDoWhat>(doWhat);
}

//Handle user choice and call the right function
void whatToDo(vector <stClientInfo>& vClientsInfo, stClientInfo& clientInfo) {
	switch (readNumThatShowWhatToDo())
	{
	case enDoWhat::ShowClientList: {
		system("cls");
		showClientsScreen(vClientsInfo);
		break;
	}
	case enDoWhat::AddNewClient: {
		system("cls");
		addNewClientScreen(vClientsInfo, clientInfo);
		break;
	}
	case enDoWhat::DeleteClient: {
		system("cls");
		deleteClientScreen(vClientsInfo);
		break;
	}
	case enDoWhat::UpdateClientInfo: {
		system("cls");
		updateClientInfoScreen(vClientsInfo, clientInfo);
		break;
	}
	case enDoWhat::FindClient: {
		system("cls");
		findClientScreen(vClientsInfo);
		break;
	}
	case enDoWhat::Exit: {
		exitScreen();
		return;
	}
	}

	goBackToMainMenu();
}

//Print the main menu options on screen
void printMainMenuScreen() {
	system("cls");
	cout << "=================================================\n";
	cout << "                Main Menu Screen\n";
	cout << "=================================================\n";
	cout << "         [1] Show Client List.\n";
	cout << "         [2] Add New Client.\n";
	cout << "         [3] Delete Client.\n";
	cout << "         [4] Update Client Info.\n";
	cout << "         [5] Find Client.\n";
	cout << "         [6] Exit.\n";
	cout << "=================================================\n";
}

//Loads client data and handles the main menu loop
void MainMenuScreen() {
	stClientInfo clientInfo{};
	vector <stClientInfo> vClientsInfo = loadClientInfoFromFile();
	printMainMenuScreen();
	whatToDo(vClientsInfo, clientInfo);
}

//Entry point of the program
int main() {
	MainMenuScreen();
}
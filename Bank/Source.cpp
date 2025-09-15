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

//Global flag that indicates whether the program should exit
bool exitProgram = false;

//Enum that represents the main operations the program can perform
enum enMainOperations {ShowClientList = 1, AddNewClient = 2, DeleteClient = 3,
					   UpdateClientInfo = 4, FindClient = 5, Transactions = 6, Exit = 7};

//Enum that represents the transaction operations
enum enTransactions{Deposit = 1, Withdraw = 2, TotalBalances = 3, MainMenu = 4};

//Enum that maps each client field
enum enClientInfo{AccountNum, PinCode, Name, Phone, Balance};

//Enum that specifies the validation mode for account numbers
enum enAccountValidationMode {MustExist, MustNotExist};

//Represents a single client's data
struct stClientInfo {
	string AccountNum;
	short PinCode;
	string name;
	string Phone;
	int balance;
};

//Displays the Main Menu and handles user operations
void mainMenuScreen(vector<stClientInfo>& vClientsInfo, stClientInfo& clientInfo);

//Show Transactions Menu and handle user choice
void transactionsMenuScreen(vector<stClientInfo>& vClientsInfo, stClientInfo& clientInfo);

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

//prompt user for account number and validate it depending on the mode (must exist / must not exist)
void promptAndValidateAccountNum(const vector<stClientInfo>& vClientsInfo, stClientInfo& clientInfo, short& index,
						enAccountValidationMode AccountValidationMode = enAccountValidationMode::MustExist) {
	cout << "\n\nPlease enter Account Number: ";
	cin >> clientInfo.AccountNum;


	index = findClientIndexByAccountNum(vClientsInfo, clientInfo.AccountNum);

	if (AccountValidationMode == enAccountValidationMode::MustNotExist) 
	{
		while (index != -1) {
			cout << "\nClient with [" << clientInfo.AccountNum << "] already exists! , Enter another Account Number: ";
			cin >> clientInfo.AccountNum;
			index = findClientIndexByAccountNum(vClientsInfo, clientInfo.AccountNum);
		}
	}
	else 
	{
		while (index == -1) {
			cout << "Client with Account Number [" << clientInfo.AccountNum
				<< "] is not found!, Enter an Existing Account Number: ";
			cin >> clientInfo.AccountNum;
			index = findClientIndexByAccountNum(vClientsInfo, clientInfo.AccountNum);
		}
	}
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

//Return to the Main Menu unless exitProgram is set to true
void goBackToMainMenu(vector<stClientInfo>& vClientsInfo, stClientInfo& clientInfo) {
	if (exitProgram) return;

	cout << "\n\nPress any key to go back to Main Menu...";
	system("pause>0");
	mainMenuScreen(vClientsInfo, clientInfo);
}

//Print the header of the client list screen
void printClientListScreenHeader(const vector<stClientInfo>& vClientsInfo) {
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
void showClientsScreen(const vector<stClientInfo>& vClientsInfo) {
	printClientListScreenHeader(vClientsInfo);
	cout << '\n';

	if (vClientsInfo.empty()) {
		MyLib::printCentered("No clients available.");
		cout << "\n\n";
		MyLib::fillLineWithHypens();
		return;
	}

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
	short index;

	cout << "-----------------------------------------\n";
	cout << "          Add New Client Screen\n";
	cout << "-----------------------------------------\n";
	cout << "Adding New Client:\n";
	
	do
	{
		promptAndValidateAccountNum(vClientsInfo, clientInfo, index, enAccountValidationMode::MustNotExist);

		enterClientInfo(clientInfo);

		vClientsInfo.push_back(clientInfo);

		fstream ClientsInfo;
		ClientsInfo.open(ClientsFileName, ios::out | ios::app);
		if (ClientsInfo.is_open())
		{
			ClientsInfo << convertRecordToLine(vClientsInfo[vClientsInfo.size() - 1]) << '\n';
		}
		ClientsInfo.close();

	} while (readYesNo("\n\nClient added successfully. Do you want to add another client? Y/N? ") == 'Y');
}

//Delete a client by account number
void deleteClientScreen(vector<stClientInfo>& vClientsInfo, stClientInfo& clientInfo) {
	short index;
	char deleteAnotherClient;

	cout << "-----------------------------------------\n";
	cout << "           Delete Client Screen\n";
	cout << "-----------------------------------------\n";

	do
	{
		deleteAnotherClient = 'N';
		promptAndValidateAccountNum(vClientsInfo, clientInfo, index);

		printClientInfo(vClientsInfo, index);

		if (readYesNo("\n\nAre you sure you want to delete this client? Y/N? ") == 'Y') {
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
	short index;
	char updateAnotherClientInfo;

	cout << "-----------------------------------------\n";
	cout << "        Update Client Info Screen\n";
	cout << "-----------------------------------------\n";

	do
	{
		updateAnotherClientInfo = 'N';
		promptAndValidateAccountNum(vClientsInfo, clientInfo, index);

		printClientInfo(vClientsInfo, index);

		if (readYesNo("\n\nAre you sure you want to update this client? Y/N? ") == 'Y') {
			cout << "\n\n";
			updateClientInfo(clientInfo, clientInfo.AccountNum);
			vClientsInfo[index] = clientInfo;
			saveClientsInfoToFile(vClientsInfo);

			updateAnotherClientInfo = readYesNo("\n\nClient Updated Successfully. Do you want to update another client? Y/N? ");
		}
	} while (toupper(updateAnotherClientInfo) == 'Y');
}

//Find and display client details by account number
void findClientScreen(const vector <stClientInfo>& vClientsInfo, stClientInfo& clientInfo) {
	short index;

	cout << "-----------------------------------------\n";
	cout << "            Find Client Screen\n";
	cout << "-----------------------------------------\n";

	do
	{
		promptAndValidateAccountNum(vClientsInfo, clientInfo, index);

		printClientInfo(vClientsInfo, index);

	} while (readYesNo("\n\nDo you want to see another client details? Y/N? ") == 'Y');
}

//Exit program (display exit message, pauses, and sets exitProgram flag to true)
void exitScreen() {
	cout << "-----------------------------------------\n";
	cout << "             Program Ends :-)\n";
	cout << "-----------------------------------------\n";
	system("pause>0");
	exitProgram = true;
}

//Deposit money into a client account
void depositScreen(vector<stClientInfo>& vClientsInfo, stClientInfo& clientInfo) {
	short index;
	short depositAmount;

	cout << "-----------------------------------------\n";
	cout << "              Deposit Screen\n";
	cout << "-----------------------------------------\n";
	
	promptAndValidateAccountNum(vClientsInfo, clientInfo, index);

	printClientInfo(vClientsInfo, index);

	cout << "\nPlease enter deposit amount: ";
	cin >> depositAmount;

	if (readYesNo("\n\nAre you sure you want perform this transactoin? Y/N? ") == 'Y') {
		vClientsInfo[index].balance += depositAmount;
		saveClientsInfoToFile(vClientsInfo);
	}
}

//Withdraw money from a client account
void withdrawScreen(vector<stClientInfo>& vClientsInfo, stClientInfo& clientInfo) {
	short index;
	short withdrawAmount;

	cout << "-----------------------------------------\n";
	cout << "              Withdraw Screen\n";
	cout << "-----------------------------------------\n";

	promptAndValidateAccountNum(vClientsInfo, clientInfo, index);

	printClientInfo(vClientsInfo, index);

	cout << "\nPlease enter withdraw amount: ";
	cin >> withdrawAmount;
	while (withdrawAmount > vClientsInfo[index].balance)
	{
		cout << "\nAmount exceeds the balance, you can withdraw up to : " << vClientsInfo[index].balance;
		cout << "\nPlease enter another amount: ";
		cin >> withdrawAmount;
	}

	if (readYesNo("\n\nAre you sure you want perform this transactoin? Y/N? ") == 'Y') {
		vClientsInfo[index].balance -= withdrawAmount;
		saveClientsInfoToFile(vClientsInfo);
	}
}

//Print the header of the total balances screen
void printTotalBalancesScreenHeader(const vector<stClientInfo>& vClientsInfo) {
	string title = "Balances List [";
	title += to_string(vClientsInfo.size());
	title += "] Client(s).";

	MyLib::printCentered(title);
	cout << '\n';

	MyLib::fillLineWithHypens();
	cout << left << setw(30) << "| Account Number";
	cout << left << setw(50) << "| Client Name";
	cout << "| Balance\n";
	MyLib::fillLineWithHypens();
}

//Show all clients' balances and display the total balance at the end
void totalBalancesScreen(const vector<stClientInfo> vClientsInfo) {
	int totalBalances = 0;

	printTotalBalancesScreenHeader(vClientsInfo);
	cout << '\n';

	if (vClientsInfo.empty()) {
		MyLib::printCentered("No clients available.");
		cout << "\n\n";
		MyLib::fillLineWithHypens();
		return;
	}

	for (int i = 0; i < vClientsInfo.size(); i++)
	{
		totalBalances += vClientsInfo[i].balance;

		cout << "| "; cout << left << setw(28) << vClientsInfo[i].AccountNum;
		cout << "| "; cout << left << setw(48) << vClientsInfo[i].name;
		cout << "| "; cout << vClientsInfo[i].balance << '\n';
	}
	cout << '\n';
	MyLib::fillLineWithHypens();
	
	MyLib::printCentered("Total Balances = " + to_string(totalBalances));
}

//Read user choice (1–4) from the Transactions Menu Screen
enTransactions readNumForTransactionOperations() {
	short choice;

	cout << "Choose what do you want to do? [1 to 4]? ";
	cin >> choice;
	while (cin.fail() || choice < 1 || choice > 4) {
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "Invalid number, Enter 1, 2, 3 or 4: ";
		cin >> choice;
	}

	return static_cast<enTransactions>(choice);
}

//Return to the Transactions Menu after finishing an operation
void goBackToTransactionsScreen(vector<stClientInfo>& vClientsInfo, stClientInfo& clientInfo) {
	cout << "\n\nPress any key to go back to Transactions Menu...";
	system("pause>0");
	transactionsMenuScreen(vClientsInfo, clientInfo);
}

//Handle transaction menu operations
void transactionOperations(vector<stClientInfo>& vClientsInfo, stClientInfo& clientInfo) {
	switch (readNumForTransactionOperations())
	{
	case enTransactions::Deposit: {
		system("cls");
		depositScreen(vClientsInfo, clientInfo);
		break;
	}

	case enTransactions::Withdraw: {
		system("cls");
		withdrawScreen(vClientsInfo, clientInfo);
		break;
	}

	case enTransactions::TotalBalances: {
		system("cls");
		totalBalancesScreen(vClientsInfo);
		break;
	}

	case enTransactions::MainMenu: {
		mainMenuScreen(vClientsInfo, clientInfo);
		return;
	}
	}

	goBackToTransactionsScreen(vClientsInfo, clientInfo);
}

//Print the Transactions Menu Screen
void printTransactionsMenuScreen() {
	cout << "=================================================\n";
	cout << "             Transaction Menu Screen\n";
	cout << "=================================================\n";
	cout << "       [1] Deposit.\n";
	cout << "       [2] Withdraw.\n";
	cout << "       [3] Total Balances.\n";
	cout << "       [4] Main Menu.\n";
	cout << "=================================================\n";
}

//Show Transactions Menu and handle user choice
void transactionsMenuScreen(vector<stClientInfo>& vClientsInfo, stClientInfo& clientInfo) {
	system("cls");
	printTransactionsMenuScreen();
	transactionOperations(vClientsInfo, clientInfo);
}

//Read user choice (1–7) from the Main Menu Screen
enMainOperations readNumforMainOperations() {
	short choice;

	cout << "Choose what do you want to do? [1 to 7]? ";
	cin >> choice;
	while (cin.fail() || choice < 1 || choice > 7) {
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "Invalid number, Enter 1, 2, 3, 4, 5, 6 or 7: ";
		cin >> choice;
	}

	return static_cast<enMainOperations>(choice);
}

//Handle user choice and call the right function
void mainOperations(vector <stClientInfo>& vClientsInfo, stClientInfo& clientInfo) {
	switch (readNumforMainOperations())
	{
	case enMainOperations::ShowClientList: {
		system("cls");
		showClientsScreen(vClientsInfo);
		break;
	}

	case enMainOperations::AddNewClient: {
		system("cls");
		addNewClientScreen(vClientsInfo, clientInfo);
		break;
	}

	case enMainOperations::DeleteClient: {
		system("cls");
		deleteClientScreen(vClientsInfo, clientInfo);
		break;
	}

	case enMainOperations::UpdateClientInfo: {
		system("cls");
		updateClientInfoScreen(vClientsInfo, clientInfo);
		break;
	}

	case enMainOperations::FindClient: {
		system("cls");
		findClientScreen(vClientsInfo, clientInfo);
		break;
	}

	case enMainOperations::Transactions: {
		system("cls");
		transactionsMenuScreen(vClientsInfo, clientInfo);
		break;
	}

	case enMainOperations::Exit: {
		system("cls");
		exitScreen();
		return;
	}
	}

	goBackToMainMenu(vClientsInfo, clientInfo);
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
	cout << "         [6] Transactions.\n";
	cout << "         [7] Exit.\n";
	cout << "=================================================" << endl;
}

//Displays the Main Menu and handles user operations
void mainMenuScreen(vector<stClientInfo>& vClientsInfo, stClientInfo& clientInfo) {
	if (exitProgram) return;

	printMainMenuScreen();
	mainOperations(vClientsInfo, clientInfo);
}

//Loads client data from file and starts the Main Menu loop
void Bank() {
	stClientInfo clientInfo{};
	vector <stClientInfo> vClientsInfo = loadClientInfoFromFile();

	mainMenuScreen(vClientsInfo, clientInfo);
}

//Entry point of the program
int main() {
	Bank();
}
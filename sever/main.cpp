//#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <cstring>
#include <fstream>

using namespace std;

#pragma comment (lib,"ws2_32.lib")

typedef struct setting
{
	char nameFile[50] = "";
	string path;
	string ipAddress;
	int	port;
	string path_result; 
	unsigned long long size = 0; 
	string a = "\\"; // "\"
}set;

int main()
{
	set st1;
	cout << "Enter port: " << endl;
	cin >> st1.port;
	getchar();

	// Init winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "can't Initialize winsock! Quitting" << endl;
		return 0;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);

	// Bind the ip address and port to the socket
	sockaddr_in hint; //sockaddr_in is a data type in the C++ socket library used to represent the IP address and port number of an endpoint
	hint.sin_family = AF_INET;
	hint.sin_port = htons(st1.port);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;  //could also use inet_pton

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell me winsock the socket is for listening
	listen(listening, SOMAXCONN);

	// Waiting for connect
	sockaddr_in client; 
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST]; //client's remote name
	char service[NI_MAXHOST]; //service (i.e.port) the client is connect on

	ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);
	
	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connect on port " << service << endl;
	}
	else
	{	//get the IP address of the client (from the sin_addr field in sockaddr_in), and save it in the server variable. Then the code enters the client's IP address (stored in the server variable) and the port number the client connects to (from the sin_port field in sockaddr_in)
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << "connected on port" << ntohs(client.sin_port) << endl;
	}

	// Close listening socket
	closesocket(listening);

	int err = recv(clientSocket, (char*)st1.nameFile, 50, 0);
	if (err <= 0)
	{
		printf("recv: %d\n", WSAGetLastError());
		closesocket(clientSocket);
	}
	// Print the file name
	printf("recv %d bytes [OK]\n", err);
	cout << "Filename: " << st1.nameFile << endl;
	
	//Enter the path to the directory where it will be saved
	cout << "Enter the path to the directory where it will be saved : " << endl;
	getline(cin, st1.path);

	// Print the file path
	st1.path_result = st1.path + st1.a + st1.nameFile;
	cout <<"path_result: " << st1.path_result << endl; 

	// Receive the file size
	err = recv(clientSocket, (char*)&st1.size, sizeof(st1.size), 0);
	if (err <= 0)
	{
		printf("recv: %d\n", WSAGetLastError());
		closesocket(clientSocket);
	}
	printf("recv %d bytes [OK]\n", err);
	cout << "size of file:" << st1.size << endl; //in size

	// Calculate the number of full blocks and the remaining bytes
	unsigned long long phannguyen_cnt = st1.size / 4096;
	int phandu_block = st1.size - (phannguyen_cnt * 4096);
	unsigned long long totalBytesReceived = 0;

	cout << "phannguyen: " << phannguyen_cnt << endl;
	cout << "phandu: " << phandu_block << endl;

	// Create a buffer to store the data
	char* buffer = new char[4096];

	// Open file
	ofstream file(st1.path_result, ios::binary | ios::out);
	if (!file.is_open()) {
		cerr << "Error opening file: " << st1.path_result << endl;
		delete[] buffer;
		return -1;
	}

	// Receive data block by block
	for (unsigned long long i = 0; i < phannguyen_cnt; i++) {
		memset(buffer, 0, 4096);
		int bytesRead = recv(clientSocket, buffer, 4096, 0);
		if (bytesRead <= 0) {
			cerr << "Error receiving data." << endl;
			break; // Exit if there's an error or no data received
		}
		totalBytesReceived += bytesRead;
		file.write(buffer, bytesRead);
	}

	// Handle the remaining bytes if any
	if (phandu_block > 0) {
		memset(buffer, 0, 4096);
		int bytesRead = recv(clientSocket, buffer, phandu_block, 0);
		if (bytesRead <= 0) {
			cerr << "Error receiving remaining data." << endl;
		}
		else {
			totalBytesReceived += bytesRead;
			file.write(buffer, bytesRead);
		}
	}

	cout << "Total bytes received: " << totalBytesReceived << endl;

	if (totalBytesReceived != st1.size) {
		cerr << "Warning: File transfer incomplete. Expected " << st1.size << " bytes but received " << totalBytesReceived << " bytes." << endl;
	}

	// Close file
	file.close();
	// Free
	delete[] buffer;
	// Close socket
	closesocket(clientSocket);
	// Cleanup winsock
	WSACleanup();
	return 0;
}

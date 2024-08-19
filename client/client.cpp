#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <cstring>
#include <WS2tcpip.h>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

typedef struct setting
{
	char nameFile[50] = "";
	string path;
	string ipAddress; 
	long long size; 
	int	port; 
}set;

int main()
{
	set st1;
	cout << "Enter the server IP address: " << endl;
	getline(cin, st1.ipAddress);
	cout << "Enter the port: " << endl;
	cin >> st1.port;
	getchar();

	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err #" << wsResult << endl;
		return -1;
	}

	// Initialize socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(st1.port);
	inet_pton(AF_INET, st1.ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return -1;
	}

	cout << "Enter the path to the file to be transmitted: " << endl;
	getline(cin, st1.path);

	// Get the file name from the path
	size_t lastSlashPosition = st1.path.rfind('\\');
	if (lastSlashPosition != string::npos)
	{

		string directory = st1.path.substr(lastSlashPosition + 1);

		cout << "name: " << directory << std::endl;
		strcpy(st1.nameFile, directory.c_str());
	}
	else {
		cout << "name: " << st1.path << endl;
	}
	// Send the file name to the server
	int err = send(sock, (char*)st1.nameFile, 50, 0);
	if (err <= 0)
	{
		printf("send: %d\n", WSAGetLastError());
	}
	printf("send %d bytes [OK]\n", err);

	ifstream file(st1.path, ios::in | ios::binary);
	file.seekg(0, ios::end);
	st1.size = file.tellg();
	file.seekg(0, ios::beg );
	char* buffer = new char[st1.size];
	file.read(buffer,st1.size);
	file.close();
	
	// Send the file size to server
	long long* fsize = &st1.size;
	err = send(sock, (char*)fsize, sizeof(st1.size), 0);
	if (err <= 0)
	{
		printf("send: %d\n", WSAGetLastError());
	}
	printf("send %d bytes [OK]\n", err);
	cout << "Size of files: " << st1.size << endl;

	 // Send data block by block to server
	unsigned long long phannguyen_cnt = st1.size / 4096;
	int phandu_block = st1.size - (phannguyen_cnt * 4096);
	cout << "Number of 4096 blocks: " << phannguyen_cnt << endl;
	cout << "Remainder blocks: " << phandu_block;

	// 4096 blocks
	for (unsigned long long i = 0; i < phannguyen_cnt; i++) {
		err = send(sock, buffer + i * 4096, 4096, 0);
		if (err <= 0)
		{
			printf("send: %d\n", WSAGetLastError());
			break;
		}
	}
	// Remaining block
	if (phandu_block > 0) {
		err = send(sock,buffer + phannguyen_cnt * 4096, phandu_block,0);
		if (err <= 0)
		{
			printf("send: %d\n", WSAGetLastError());
		}
	}

	// Free 
	delete[] buffer;
	//close socket
	closesocket(sock);
	//cleanup winsock
	WSACleanup();
	return 0;
}

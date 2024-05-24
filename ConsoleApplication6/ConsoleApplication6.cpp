#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
using namespace std;
int main()
{
	WSADATA wsaData;
	ADDRINFO* addResult;
	ADDRINFO hints;
	SOCKET ConnectSocket = INVALID_SOCKET;
	const char* sendBuffer = "Hello from Client";
	const char* sendBuffer2 = "Hello from client 2";
	char recvBuffer[512];


	int result = WSAStartup(MAKEWORD(2,2), &wsaData);

	if (result != 0) {
		std::cout << "WSAStartup failes result" << std::endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	result = getaddrinfo("localhost", "666", &hints,&addResult);
	if (result != 0) {
		std::cout << "Getaddrinfo wit error" << std::endl;
		WSACleanup();
		return 1;
	}

	ConnectSocket = socket(addResult->ai_family, addResult->ai_socktype, addResult->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Socket creation with " << std::endl;
		WSACleanup();
		return 1;
	}

	result = connect(ConnectSocket, addResult->ai_addr, (int)addResult->ai_addrlen);
	if (result == SOCKET_ERROR) {
		std::cout << "Unable to connect to server" << std::endl;
		closesocket(ConnectSocket);
		freeaddrinfo(addResult);
		WSACleanup();
		return 1;
	}

	result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
	if (result == SOCKET_ERROR) {
		cout << "Send failed, error: " << result << endl;
		closesocket(ConnectSocket);
		freeaddrinfo(addResult);
		WSACleanup();
		return 1;
	}

	std::cout << "Data sent succsesfully" << result << std::endl;

	result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
	if (result == SOCKET_ERROR) {
		cout << "Send failed, error: " << result << endl;
		closesocket(ConnectSocket);
		freeaddrinfo(addResult);
		WSACleanup();
		return 1;
	}

	std::cout << "Data sent succsesfully" <<result<< std::endl;
	
	result = shutdown(ConnectSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		cout << "shutdown error <3" << endl;
		freeaddrinfo(addResult);
		WSACleanup();//очситка WSAStartup
		return 1;
	}

	do {
		ZeroMemory(recvBuffer, 512);
		result = recv(ConnectSocket, recvBuffer, 512, 0);
		if (result > 0) {
			cout << "Received " << result << "bytes" << endl;
			cout << "Received data " << recvBuffer << endl;

		}
		else if (result == 0)
			cout << "Connection closed" << endl;

		
	else {
		cout << "Received failed " << endl;
		closesocket(ConnectSocket);

	}
	} while (result > 0);
	result = shutdown(ConnectSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		cout << "shutdown  error " << endl;
		freeaddrinfo(addResult);
		WSACleanup();
		return 1;
	}
	closesocket(ConnectSocket);
	freeaddrinfo(addResult);
	WSACleanup();
	return 0;


}


#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

int main()
{
	WSADATA wsaData;
	ADDRINFO* addResult;
	ADDRINFO hints;
	SOCKET ClientSocket = INVALID_SOCKET;
	SOCKET ListentSocket = INVALID_SOCKET;

	const char* sendBuffer = "Hello from server";
	char recvBuffer[512];

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0) {
		std::cout << "WSAStartup failes result" << std::endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo(NULL, "666", &hints, &addResult);
	if (result != 0) {
		std::cout << "Getaddrinfo wit error" << std::endl;
		WSACleanup();
		return 1;
	}

	ListentSocket = socket(addResult->ai_family, addResult->ai_socktype, addResult->ai_protocol);
	if (ListentSocket == INVALID_SOCKET) {
		std::cout << "Socket creation with " << std::endl;
		freeaddrinfo(addResult);
		return 1;
	}

	result = bind(ListentSocket, addResult->ai_addr, (int)addResult->ai_addrlen);
	if (result == SOCKET_ERROR) {
		std::cout << "Binding connect failed" << std::endl;
		closesocket(ListentSocket);
		freeaddrinfo(addResult);
		WSACleanup();
		return 1;
	}

	result = listen(ListentSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		std::cout << "Listening socket failed" << std::endl;
		closesocket(ListentSocket);
		freeaddrinfo(addResult);
		WSACleanup();
		return 1;
	}

	ClientSocket = accept(ListentSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		std::cout << "Accepting socket failed" << std::endl;
		closesocket(ListentSocket);
		freeaddrinfo(addResult);
		WSACleanup();
		return 1;
	}

	closesocket(ListentSocket);
	do {
		ZeroMemory(recvBuffer, 512);
		result = recv(ClientSocket, recvBuffer, 512, 0);
		if (result > 0) {
			std::cout << "Recieved " << result << "bytes" << std::endl;
			std::cout << "Recieved data" << recvBuffer << std::endl;

			result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0);

			if (result == SOCKET_ERROR) {
				std::cout << "Failed to send data back" << std::endl;
				closesocket(ClientSocket);
				freeaddrinfo(addResult);
				WSACleanup();
			}
		}
		else if (result == 0)
			std::cout << "Connection closing..." << std::endl;

		else {
			std::cout << "recv failed with error" << std::endl;
			closesocket(ClientSocket);
			freeaddrinfo(addResult);
			WSACleanup();
			return 1;
		}
	} while (result > 0);

	result = shutdown(ClientSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		closesocket(ClientSocket);
		freeaddrinfo(addResult);
		WSACleanup();
		return 1;
	}
	closesocket(ClientSocket);
	freeaddrinfo(addResult);
	WSACleanup();
	return 0;
}
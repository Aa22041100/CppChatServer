// CppChatClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

// winsock lib
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")

// socket info
#define DEFAULT_PORT "27015"

// define msg buffer length
#define DEFAULT_BUFLEN 512

int __cdecl main(int argc, char **argv)
{
	WSADATA wsaData;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	int iResult;
	SOCKET ConnectSocket = INVALID_SOCKET;

	// msg values
	int recvbuflen = DEFAULT_BUFLEN;
	char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];

	// init winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
	
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// resolve the server address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// attempt to connect to the first address returned by the call to getaddrinfo
	ptr = result;

	// create a socket for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// connect to server
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	// to-do: try the next ip which is returned by getaddrinfo
	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	// send an initial buffer
	iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", iResult);
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);
	
	// shutdown connection after sending since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", iResult);
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// receive data until the server closes the connection
	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
		}
		else if (iResult == 0) {
			printf("Connection closed\n");
		}
		else {
			printf("recv failed: %d\n", WSAGetLastError());
		}
	} while (iResult > 0);

	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", iResult);
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

    return 0;
}
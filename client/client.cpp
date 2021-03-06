


#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  

#include <process.h>  
#include <WinSock2.h>

#include <windows.h>  

#include <iostream>

#pragma comment(lib, "ws2_32.lib")  

unsigned WINAPI SendMsg(void * arg);
unsigned WINAPI RecvMsg(void * arg);

int haveread = 0;


#define MESSAGE_SIZE 100

char msg[MESSAGE_SIZE];

using namespace std;

#define NAME_SIZE 20

char NAME[NAME_SIZE];

enum MessageType {
	req_con, req_discon, send_mes
};

#pragma pack(push,1)
typedef struct PACKAGE {
//	int nameLength;
	MessageType msgType;
	char message[MESSAGE_SIZE];
}PACKAGE;
#pragma pack(pop)



int main(int argc, char *argv[])
{

	printf("input your chat name \n");
	scanf_s("%s", NAME,sizeof(NAME));
	
	

	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
		

	hSock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr("127.0.0.1");// Local Host LOOP
	servAdr.sin_port = htons(1234);

	if (connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) != SOCKET_ERROR) {
		PACKAGE package;
		PACKAGE *pPack = &package;

		char dest[ NAME_SIZE];
		dest[0] = '\0';

		strcat_s(dest, sizeof(dest), NAME);
		//strcat_s(dest, sizeof(msg), msg);
		//pPack->nameLength = strlen(NAME);
		pPack->msgType = MessageType::req_con;

		strcpy_s(pPack->message, MESSAGE_SIZE, dest);
	
		///////////
		int buffSize = sizeof(package) - MESSAGE_SIZE + strlen(pPack->message) + 1;
		//cout << buffSize << endl;
		 send(hSock, (char *)pPack, buffSize, 0);
	}


	hSndThread =
		(HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSock, 0, NULL);
	hRcvThread =
		(HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSock, 0, NULL);

	WaitForSingleObject(hSndThread, INFINITE);
	WaitForSingleObject(hRcvThread, INFINITE);
	closesocket(hSock);
	WSACleanup();
	system("pause");
	return 0;
}

unsigned WINAPI SendMsg(void * arg)   // send thread main  
{
	SOCKET sock = *((SOCKET*)arg);
	
	//char padd[2];
	//fgets(padd, 2, stdin);
	//printf("\n send message:");
	while (1)
	{
		{
			PACKAGE package;
			PACKAGE *pPack = &package;
			char dest[MESSAGE_SIZE + NAME_SIZE];
			dest[0] = '\0';

			//cout << "\n"<<"send message:" << endl;
			memset(msg, 0, sizeof(msg));
			//fgets(msg, MESSAGE_SIZE, stdin);
			cin.getline(msg,sizeof(msg));
			
			string a = msg;
			if (a.compare("/bye")== 0) {
				strcat_s(dest, sizeof(dest), NAME);
				strcpy_s(pPack->message, MESSAGE_SIZE, dest);
				pPack->msgType = MessageType::req_discon;
				int buffSize = sizeof(package) - MESSAGE_SIZE + strlen(pPack->message) + 1;
				int result = send(sock, (char *)pPack, buffSize, 0);

				exit(0);
			}


			strcat_s(dest, sizeof(dest), NAME);
			strcat_s(dest, sizeof(dest), ":");
			strcat_s(dest, sizeof(msg), msg);
			
			strcpy_s(pPack->message, MESSAGE_SIZE, dest);
			pPack->msgType = MessageType::send_mes;

			int buffSize = sizeof(package) - MESSAGE_SIZE + strlen(pPack->message) + 1 ;
			int result = send(sock, (char *)pPack, buffSize, 0);
		
		}
	}
	return NULL;
}

unsigned WINAPI RecvMsg(void * arg)  // read thread main  
{
	SOCKET sock = *((SOCKET*)arg);
	char name_msg[NAME_SIZE + MESSAGE_SIZE];
	
	
	LPWSABUF recvBuff;
	DWORD byteTrans = 0;
	DWORD flags = 0;
	//LPWSABUF *precvBuff = recvBuff;
		
		while (1)
	{

			recvBuff = (LPWSABUF)malloc(sizeof(name_msg));
			recvBuff->buf = name_msg;
			recvBuff->len = 120;
			
			WSARecv(sock, recvBuff,
			1, &byteTrans, &flags, NULL, NULL);
			
			PACKAGE* pPack = (PACKAGE*)recvBuff->buf;
			
			cout <<+"\n"<< pPack->message << endl;
	
		//	cout << "\n" << "send message:" << endl;

			free(recvBuff);
	
	}
	
}



#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <windows.h>  
#include <process.h>   

#include <iostream>

#define BUF_SIZE 1000  
#define NAME_SIZE 20  

#pragma comment(lib, "ws2_32.lib")  

unsigned WINAPI SendMsg(void * arg);
unsigned WINAPI RecvMsg(void * arg);
void ErrorHandling(char * msg);

int haveread = 0;
char NAME[50];
char ANAME[50];
char msg[BUF_SIZE];

using namespace std;

int main(int argc, char *argv[])
{

	printf("input your chat name ");
	scanf_s("%s", NAME,sizeof(NAME));
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hSock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr("127.0.0.1");// Local Host LOOP
	servAdr.sin_port = htons(1234);

	if (connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("connect() error");

	int resultsend;
	cout<<"Welcome to joining our chatting room!\n";
	sprintf_s(ANAME, "[%s]", NAME);

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
	char name_msg[NAME_SIZE + BUF_SIZE];
	char padd[2];
	fgets(padd, 2, stdin);
	printf("\n send message:");
	while (1)
	{
		{
			fgets(msg, BUF_SIZE, stdin);
			if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
			{
				closesocket(sock);
				exit(0);
			}
			sprintf_s(name_msg, "[%s] %s", NAME, msg);
			char numofmsg = strlen(name_msg) + '0';
			char newmsg[100]; newmsg[0] = numofmsg; newmsg[1] = 0;
			strcat_s(newmsg, name_msg);
			int result = send(sock, newmsg, strlen(newmsg), 0);
			if (result == -1)return -1;
		}
	}
	return NULL;
}

unsigned WINAPI RecvMsg(void * arg)  // read thread main  
{
	SOCKET sock = *((SOCKET*)arg);
	char name_msg[NAME_SIZE + BUF_SIZE];
	int str_len = 0;
	while (1)
	{
		{
			char lyfstr[1000] = { 0 };
			int totalnum = 0;
			str_len = recv(sock, name_msg, 1, 0);
			if (str_len == -1)  
			{
				printf("return -1\n");
				return -1;
			}
			if (str_len == 0) 
			{
				printf("return 0\n");
				return 0; 
			}
			totalnum = name_msg[0] - '0';
			int count = 0;

			do
			{
				str_len = recv(sock, name_msg, 1, 0);

				name_msg[str_len] = 0;

				if (str_len == -1)
				{
					printf("return -1\n");
					return -1;
				}
				if (str_len == 0)
				{
					printf("return 0\n");
					return 0; 
				}
				strcat_s(lyfstr, name_msg);
				count = str_len + count;

			} while (count < totalnum);

			lyfstr[count] = '\0';
			printf("\n");
			strcat_s(lyfstr, "\n");
			fputs(lyfstr, stdout);
			printf(" send message:");
			fflush(stdout);
			memset(name_msg, 0, sizeof(char));
		}
	}
	return NULL;
}

void ErrorHandling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
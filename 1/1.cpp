// FileClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#include "ws2tcpip.h"
#include <conio.h>
using namespace std;
int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct addrinfo * result;
	SOCKADDR_IN address;
	char domainName[128];

	char *ip_to_enter;
	char ip_to_connect[64];


	while (1) {
		printf("Enter domain name or ip of ftp server: ");
		scanf("%s", domainName);
		// Phan giai ten mien
		int res_domain = getaddrinfo(domainName, "http", NULL, &result);
		if (res_domain == 0) {
			memcpy(&address, result->ai_addr, result->ai_addrlen);
			ip_to_enter = inet_ntoa(address.sin_addr);
			sprintf(ip_to_connect, "%s", ip_to_enter);
			break;
		}
		else
			printf("Domain name or IP is incorrect\n");
	}

	printf("Connecting...\n");
	// Done connecting
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip_to_connect);
	addr.sin_port = htons(21);


	int res = connect(clientSocket, (SOCKADDR *)&addr, sizeof(addr));
	if (res == SOCKET_ERROR) {
		printf("Connect failed.\n");
		system("PAUSE");
		return 0;
	}
	printf("Connect succeed.\n");

	char buf[1024];
	char buf2[1024];

	//Nhan thong tin sau khi connect xong
	int res_after_connecting = recv(clientSocket, buf, sizeof(buf), 0);
	buf[res_after_connecting] = 0;
	puts(buf);

	// Nhap id va pass
	while (1) {
		//id
		char id_to_enter[256];
		char id_to_send[256];
		printf("Enter username: ");
		scanf("%s", &id_to_enter);
		sprintf(id_to_send, "USER %s\n", id_to_enter);

		// pass
		char pass_to_enter[256];
		char pass_to_send[256];
		printf("Enter password: ");

		// Chuyen doi khi nhap password thi thanh dau *
		int i = 0;
		char a;
		for (i = 0;;)
		{
			a = _getch();
			if ((a >= 'a'&&a <= 'z') || (a >= 'A'&&a <= 'Z') || (a >= '0'&&a <= '9'))
			{
				pass_to_enter[i] = a;
				++i;
				cout << "*";
			}
			if (a == '\b'&&i >= 1)
			{
				cout << "\b \b";
				--i;
			}
			if (a == '\r')
			{
				pass_to_enter[i] = '\0';
				break;
			}
		}
		sprintf(pass_to_send, "PASS %s\n", pass_to_enter);

		send(clientSocket, id_to_send, strlen(id_to_send), 0);

		printf("\n");
		int res_id = recv(clientSocket, buf, sizeof(buf), 0);

		send(clientSocket, pass_to_send, strlen(pass_to_send), 0);

		// Nhan thong tin sau khi nhap pass
		int res_pass = recv(clientSocket, buf, sizeof(buf), 0);
		buf[res_pass] = 0;
		puts(buf);

		// Phong thu loi nhap id hoac pass sai
		char error[3];
		for (int i = 0; i <= 2; i++) {
			error[i] = buf[i];
		}
		int error_code = atoi(error);
		if (error_code != 530)
			break;
	}
	printf("\nList of commands:\n- download\n- upload\n- cd\n- quit\n- delete\n- ls\n- rename\n -mkdir\n- rmdir\n- pwd\n");

	while (1) {
			
		SOCKET data_socket;
		SOCKADDR_IN addr;
		char cmd1[56], cmd2[256], tmp[56];
		char port[64];
		char path[256];
		printf("Enter command:");
		int cmd = scanf("%s", cmd1);

		if (cmd == 1) {

			if (strcmp(cmd1, "quit") == 0) {
				char * quit = "QUIT\n";
				send(clientSocket, quit, strlen(quit), 0);
				// Nhan thong tin sau khi QUIT
				int res_cd = recv(clientSocket, buf, sizeof(buf), 0);
				buf[res_cd] = 0;
				puts(buf);
				break;
			}

			if (strcmp(cmd1, "mkdir") == 0) {
					printf("Directory name:\n");
					char dir[256];
					char dir_to_send[256];
					scanf("%s", dir);

					sprintf(dir_to_send, "MKD %s\n", dir);
					send(clientSocket, dir_to_send, strlen(dir_to_send), 0);
					// Nhan thong tin sau khi DELE
					int res_cd = recv(clientSocket, buf, sizeof(buf), 0);
					buf[res_cd] = 0;
					puts(buf);
			}

			if (strcmp(cmd1, "pwd") == 0) {

				char pwd[256] = "PWD\n";

				send(clientSocket, pwd, strlen(pwd), 0);
				// Nhan thong tin sau khi PWD
				int res_cd = recv(clientSocket, buf, sizeof(buf), 0);
				buf[res_cd] = 0;
				puts(buf);
			}

			if (strcmp(cmd1, "rmdir") == 0) {
					printf("Directory name:\n");
					char dir[256];
					char dir_to_send[256];
					scanf("%s", dir);

					sprintf(dir_to_send, "RMD %s\n", dir);
					send(clientSocket, dir_to_send, strlen(dir_to_send), 0);
					// Nhan thong tin sau khi RMD
					int res_cd = recv(clientSocket, buf, sizeof(buf), 0);
					buf[res_cd] = 0;
					puts(buf);
		
			}
			if (strcmp(cmd1, "ls") == 0) {

				char pasv[6] = "PASV\n";
				send(clientSocket, pasv, strlen(pasv), 0);
				int res_pasv = recv(clientSocket, buf, sizeof(buf), 0);
				buf[res_pasv] = 0;
				puts(buf);
				int len = res_pasv - 4;
				int a = 28 + strlen(ip_to_connect);
				int before_port_len = len - a;
				for (int i = a; i <= len; i++) {
					port[i - a] = buf[i];
				}
				port[before_port_len + 1] = 0;

				char port1[10], port2[10];
				int j;

				//port1
				for (int i = 0; i < strlen(port); i++) {
					port1[i] = port[i];

					if (port[i] == 44) {
						j = i;
						break;

					}
				}
				port1[j] = 0;

				//port2
				for (int i = j + 1; i < strlen(port); i++) {
					port2[i - j - 1] = port[i];
				}
				port2[strlen(port) - j - 1] = 0;
				int port1_i = atoi(port1);
				int port2_i = atoi(port2);

				// dataPort
				int dataPort = port1_i * 256 + port2_i;

				// Connect

				data_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

				addr.sin_family = AF_INET;
				addr.sin_addr.s_addr = inet_addr(ip_to_connect);
				addr.sin_port = htons(dataPort);
				int res_data = connect(data_socket, (SOCKADDR *)&addr, sizeof(addr));
				if (res_data == SOCKET_ERROR) {
					printf("\nConnect failed.\n");
					system("PAUSE");
					return 0;
				}
				printf("Data channel opened.\n");
		
				char * ls = "LIST\n";


					send(clientSocket, ls, strlen(ls), 0);		
					int res_ls_data = recv(data_socket, buf, sizeof(buf), 0);
					buf[res_ls_data] = 0;
					puts(buf);

					closesocket(data_socket);

					int res_ls_2 = recv(clientSocket, buf, sizeof(buf), 0);
					buf[res_ls_2] = 0;
					puts(buf);
				

			}
			// RENAME

			if (strcmp(cmd1, "rename") == 0) {
					char rename_from[256];
					char rename_to[256];
					char rename_from_to_send[256];
					char rename_to_to_send[256];
					printf("Directory or file name:\n");
					scanf("%s", rename_from);

					sprintf(rename_from_to_send, "RNFR %s\n", rename_from);
					send(clientSocket, rename_from_to_send, strlen(rename_from_to_send), 0);
					// Nhan thong tin sau khi RNFR
					int res_rnfr = recv(clientSocket, buf, sizeof(buf), 0);
					buf[res_rnfr] = 0;
					puts(buf);
					char error[3];
					for (int i = 0; i <= 2; i++) {
						error[i] = buf[i];
					}
					int error_code = atoi(error);
					if (error_code == 550)
						continue;
					printf("Doi ten thanh:\n");
					scanf("%s", rename_to);
					sprintf(rename_to_to_send, "RNTO %s\n", rename_to);
					send(clientSocket, rename_to_to_send, strlen(rename_to_to_send), 0);
					int res_rnto = recv(clientSocket, buf, sizeof(buf), 0);
					buf[res_rnto] = 0;
					puts(buf);
			}

			// DELE

			if (strcmp(cmd1, "delete") == 0) {
					printf("File name:\n");
					char location[256];
					char location_to_send[256];
					scanf("%s", location);

					sprintf(location_to_send, "DELE %s\n", location);
					send(clientSocket, location_to_send, strlen(location_to_send), 0);
					// Nhan thong tin sau khi DELE
					int res_cd = recv(clientSocket, buf, sizeof(buf), 0);
					buf[res_cd] = 0;
					puts(buf);
			}

			//CWD 

			if (strcmp(cmd1, "cd") == 0) {			
					printf("To:\n");
					char location[256];
					char location_to_send[256];
					scanf("%s", location);

					sprintf(location_to_send, "CWD %s\n", location);
					send(clientSocket, location_to_send, strlen(location_to_send), 0);
					// Nhan thong tin sau khi cd
					int res_cd = recv(clientSocket, buf, sizeof(buf), 0);
					buf[res_cd] = 0;
					puts(buf);
						
			}

			//download

			if (strcmp(cmd1, "download") == 0) {

				char file_to_download[256];
				// Lay ca 2 port

					char pasv[6] = "PASV\n";
					send(clientSocket, pasv, strlen(pasv), 0);
					int res = recv(clientSocket, buf, sizeof(buf), 0);
					buf[res] = 0;
					puts(buf);
					int len = res - 4;
					int a = 28 + strlen(ip_to_connect);
					int before_port_len = len - a;
					for (int i = a; i <= len; i++) {
						port[i - a] = buf[i];
					}
					port[before_port_len + 1] = 0;

					char port1[10], port2[10];
					int j;

					//port1
					for (int i = 0; i < strlen(port); i++) {
						port1[i] = port[i];

						if (port[i] == 44) {
							j = i;
							break;

						}
					}
					port1[j] = 0;

					//port2
					for (int i = j + 1; i < strlen(port); i++) {
						port2[i - j - 1] = port[i];
					}
					port2[strlen(port) - j - 1] = 0;
					int port1_i = atoi(port1);
					int port2_i = atoi(port2);

					// dataPort
					int dataPort = port1_i * 256 + port2_i;

					// Connect

					data_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

					addr.sin_family = AF_INET;
					addr.sin_addr.s_addr = inet_addr(ip_to_connect);
					addr.sin_port = htons(dataPort);
					int res_data = connect(data_socket, (SOCKADDR *)&addr, sizeof(addr));
					if (res_data == SOCKET_ERROR) {
						printf("\nConnect failed.\n");
						system("PAUSE");
						return 0;
					}
					printf("\nData channel opened.\n");

					printf("Save file to folder:\n");
					scanf("%s", path);

					printf("File to download:\n");
					scanf("%s", file_to_download);

					char retr_full[512];
					sprintf(retr_full, "RETR %s\n", file_to_download);
					send(clientSocket, retr_full, strlen(retr_full), 0);

					int res3 = recv(clientSocket, buf, sizeof(buf), 0);
					buf[res3] = 0;
					puts(buf);


					// Phong thu nhap ten file sai
					char error[3];
					for (int i = 0; i <= 2; i++) {
						error[i] = buf[i];
					}
					int error_code = atoi(error);
					if (error_code == 550)
						continue;
				
				FILE * downloaded;

				char dir[128];
				sprintf(dir, "%s\\%s", path, file_to_download);
				errno_t err;
				err = fopen_s(&downloaded, dir, "wb");
				if (err == 0)
				{
					char data_buf[1024];

					int res_data_recv;

					while (1) {
						res_data_recv = recv(data_socket, data_buf, sizeof(data_buf), 0);

						if (res_data_recv == SOCKET_ERROR) {
							printf("\nConnection is corrupted. Program will stop!\n");
							break;
						}
						if (res_data_recv == 0)
							break;

						fwrite(data_buf, 1, res_data_recv, downloaded);
					}
					fclose(downloaded);
					closesocket(data_socket);
					int res4 = recv(clientSocket, buf, sizeof(buf), 0);
					buf[res4] = 0;
					puts(buf);
				
				}
				else
				{
					printf("PATH ERROR\n");
					continue;
				}
				
			}

			// upload
			if (strcmp(cmd1, "upload") == 0) {
				char file_to_upload[256];

					char pasv[6] = "PASV\n";
					send(clientSocket, pasv, strlen(pasv), 0);
					int res = recv(clientSocket, buf, sizeof(buf), 0);
					buf[res] = 0;
					puts(buf);
					int len = res - 4;
					int a = 28 + strlen(ip_to_connect);
					int before_port_len = len - a;
					for (int i = a; i <= len; i++) {
						port[i - a] = buf[i];
					}
					port[before_port_len + 1] = 0;

					char port1[10], port2[10];
					int j;

					//port1
					for (int i = 0; i < strlen(port); i++) {
						port1[i] = port[i];

						if (port[i] == 44) {
							j = i;
							break;

						}
					}
					port1[j] = 0;

					//port2
					for (int i = j + 1; i < strlen(port); i++) {
						port2[i - j - 1] = port[i];
					}
					port2[strlen(port) - j - 1] = 0;
					int port1_i = atoi(port1);
					int port2_i = atoi(port2);

					// dataPort
					int dataPort = port1_i * 256 + port2_i;

					// Connect

					data_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

					addr.sin_family = AF_INET;
					addr.sin_addr.s_addr = inet_addr(ip_to_connect);
					addr.sin_port = htons(dataPort);
					int res_data = connect(data_socket, (SOCKADDR *)&addr, sizeof(addr));
					if (res_data == SOCKET_ERROR) {
						printf("\nConnect failed.\n");
						system("PAUSE");
						return 0;
					}
					printf("\nData channel opened.\n");

					printf("File location:\n");
					scanf("%s", path);

					printf("File name:\n");
					scanf("%s", file_to_upload);

					char cpy[256];
					strcpy(cpy, file_to_upload);
					char * type;
					const char s[2] = ".";
					type = strtok(cpy, s);
					type = strtok(NULL, s);
					if (strcmp(type, "txt") == 0) {
						char * ascii = "TYPE A\n";
						send(clientSocket, ascii, strlen(ascii), 0);
						int res = recv(clientSocket, buf, sizeof(buf), 0);
						buf[res] = 0;
						puts(buf);
					}
					else {
						char * binary = "TYPE I\n";
						send(clientSocket, binary, strlen(binary), 0);
						int res = recv(clientSocket, buf, sizeof(buf), 0);
						buf[res] = 0;
						puts(buf);
					}

					char retr_full[512];
					sprintf(retr_full, "STOR %s\n", file_to_upload);
					send(clientSocket, retr_full, strlen(retr_full), 0);
					
					int res3 = recv(clientSocket, buf, sizeof(buf), 0);
					buf[res3] = 0;
					puts(buf);

				FILE * uploaded;

				char dir[128];
				sprintf(dir, "%s\\%s", path, file_to_upload);

				errno_t err;
				
				err = fopen_s(&uploaded, dir, "rb");

				if (err == 0) {
					char data_buf[1024];
					int res_data_send;
					while (1) {
						res_data_send = fread(data_buf, 1, sizeof(data_buf), uploaded);

						if (res_data_send == 0)
							break;

						send(data_socket, data_buf, res_data_send, 0);

						if (res_data_send < 1024)
							break;
					}

					fclose(uploaded);
					closesocket(data_socket);
					int res4 = recv(clientSocket, buf, sizeof(buf), 0);
					buf[res4] = 0;
					puts(buf);
				}
				else {
					printf("PATH ERROR\n");
					continue;
				}
			}
			else {
				printf("\nList of commands:\n- download\n- upload\n- cd\n- quit\n- delete\n- ls\n- rename\n -mkdir\n- rmdir\n- pwd\n");
			}
		}
		else {
			printf("\nList of commands:\n- download\n- upload\n- cd\n- quit\n- delete\n- ls\n- rename\n -mkdir\n- rmdir\n- pwd\n");
		}
	}
	closesocket(clientSocket);
	WSACleanup();

	return 0;
}
	
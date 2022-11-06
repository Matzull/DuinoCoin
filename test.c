#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdbool.h>
#include "sha1.h"
#include <stdlib.h>
#include <pthread.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define SHA_DIGEST_LENGTH 20
char* serverip = "51.15.127.80";
int serverport = 2811;
char serverversion[3]; // server ver is always 3 chars

int server_Connect(char* serverIp, int serverPort)
{
	int socket_desc;
	struct sockaddr_in server;

	/* Create socket object */
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1) {
		printf("Error: Couldn't create socket\n");
		return 1;
	}

	/*Attempting to connect*/
	printf("Connecting...\n");
	server.sin_addr.s_addr = inet_addr(serverIp);
	server.sin_family = AF_INET;
    if (serverPort != -1)
    {
        server.sin_port = htons(serverPort);
    }
    int client_fd;
	if ((client_fd = connect(socket_desc, (struct sockaddr*)&server, sizeof(server))) < 0) {
		printf("Error: Couldn't connect to the server\n");
		return 1;
	}
	else printf("Connected to the server\n");

	// if (recv(socket_desc, serverversion, 3, 0) < 0) {
	// 	printf("Error: Server version couldn't be received\n");
	// 	return 1;
	// }
	// else{
	// 	serverversion[3] = 0;//Proper display of serverversion
	// 	printf("Server is on version: %s\n\n", serverversion);
	// }
	return socket_desc;
} 

bool fetchPools()
{
    char* poolIp = inet_ntoa(*((struct in_addr **) gethostbyname("server.duinocoin.com")->h_addr_list)[0]);
	int socket_desc = server_Connect(poolIp , 80);
    char serverreply [350];
    const char * request = "GET /getPool HTTP/1.0\r\nHost: www.server.duinocoin.com \r\nConnection: close\r\n\r\n";

    if (send(socket_desc, request, strlen(request), 0) < 0) {
        printf("Error: Couldn't connect to the pool.\n");
        return false;
	}

	if (recv(socket_desc, serverreply, 350, 0) < 0) {
		printf("Error: Couldn't receive pool information.\n");
		return 1;
	}

    /*Find the value for serverIp and serverPort*/
    char* json;
    json = strstr(serverreply, "\"ip\":\"") + strlen("\"ip\":\"");
    serverip = strsep(&json, ",\"");
    json = strstr(json, "\"port\":") + strlen("\"port\":");
    serverport = atoi(strsep(&json, ",\""));

    printf("Succesfully conected to pool\n");
}

int main (int argc, char **argv) {
	time_t start_t, end_t;
	double diff_t;

	char serverreply[40 + 1 + 40 + 1 + 8]; // 2x sha1s, 2x commas, difficulty, \n

    fetchPools();

    //printf("Server reply: %s\n", serverreply);//Printing serverreply

	return 0;
}
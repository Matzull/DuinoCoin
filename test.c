#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include "sha1.h"
#include <stdlib.h>
#include <pthread.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define SHA_DIGEST_LENGTH 20
#define serverip "51.15.127.80"
const unsigned int serverport = 2811;

int main (int argc, char **argv) {
	time_t start_t, end_t;
	double diff_t;

	char serverversion[4]; // server ver is always 3 chars
	char serverreply[40 + 1 + 40 + 1 + 8]; // 2x sha1s, 2x commas, difficulty, \n

	int socket_desc;
	struct sockaddr_in server;

	/* Create socket object */
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1) {
		printf("Error: Couldn't create socket\n");
		return 1;
	}

	/* Establish connection to the server */
	printf("Connecting\n");
	server.sin_addr.s_addr = inet_addr(serverip);
	server.sin_family = AF_INET;
	server.sin_port = htons(serverport);

	if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		printf("Error: Couldn't connect to the server\n");
		return 1;
	}
	else printf("Connected to the server\n");

    char job_message[17] = "JOB,Matzul,LOW,";
    job_message[16] = 0;
    if (send(socket_desc, (job_message), strlen(job_message), 0) < 0) {
			printf("Error: Couldn't send JOB message\n");
			return 1;
    }

    if (recv(socket_desc, serverreply, 40 + 1 + 40 + 1 + 8, 0) < 0) {
        printf("Error: Couldn't receive job\n");
        return 1;
    }

    printf("Server reply: %s\n", serverreply);//Printing serverreply

	return 0;
}

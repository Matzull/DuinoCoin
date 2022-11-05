#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include "sha1.h"
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define SHA_DIGEST_LENGTH 20
char serverIp[13] =  "51.15.127.80";
int serverPort = 2811;
char serverversion[3]; // server ver is always 3 chars
char serverreply[40 + 1 + 40 + 1 + 8]; // 2x sha1s, 2x commas, difficulty, \n
char *itoa (int num, char *str) {
	/* Int to string */
	if (str == NULL) return NULL;
	sprintf(str, "%d", num);
	return str;
}

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
	server.sin_port = htons(serverPort);

	if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		printf("Error: Couldn't connect to the server\n");
		return 1;
	}
	else printf("Connected to the server\n");

	if (recv(socket_desc, serverversion, 3, 0) < 0) {
		printf("Error: Server version couldn't be received\n");
		return 1;
	}
	else{
		serverversion[3] = 0;//Proper display of serverversion
		printf("Server is on version: %s\n\n", serverversion);
	}
	return socket_desc;
}

bool fetchPools(char* serverIp, int serverPort)
{
	int socket_desc = server_Connect("https://server.duinocoin.com/getPool", 8080);
	if (recv(socket_desc, serverreply, 40 + 1 + 40 + 1 + 8, 0) < 0) {
		printf("Error: Couldn't receive job\n");
		return 1;
	}
	printf("%s", serverreply);//Printing serverreply
}

int main (int argc, char **argv) {
	time_t start_t, end_t;
	double diff_t;

	char job_message[64] = "JOB,";
	char* requested_difficulty = ",LOW";
	
	char username[32] = "";

	unsigned int rejected_shares = 0;
	unsigned int accepted_shares = 0;
	unsigned int hashrate = 0;

	printf("\033[1;33md-cpuminer\n\033[1;35mby phantom32 and revox 2020-2021\n");
	printf("\033[0m----------\n");

	if (argc < 2) {
		printf("Enter your DUCO username (you can also pass it when launching the miner: ./d-cpuminer username): ");
		scanf("%s", username);
	}
	else sprintf(username, argv[1], "%s"); // Get username from sys argv
	printf("Continuing as user %s\n", username);

	/* Establish connection to the server */

	/* Combine job request message */
	strcat(job_message, username);
	//strcat(job_message, requested_difficulty);

	printf("Mining started using DUCO-S1 algorithm\n");

	while (1) {
		if (send(socket_desc, job_message, strlen(job_message), 0) < 0) {
			printf("Error: Couldn't send JOB message\n");
			return 1;
		}

		if (recv(socket_desc, serverreply, 40 + 1 + 40 + 1 + 8, 0) < 0) {
			printf("Error: Couldn't receive job\n");
			return 1;
		}
		printf("%s", serverreply);//Printing serverreply

		/* Split received data */
		char* job = strtok (serverreply, ",");
		char* work = strtok (NULL, ",");
		char* diff = strtok (NULL, "");
		char ducos1_result_string[10] = "";

		//printf("\nRecived data:\n");
		//printf("JOB: %s\n", job);
		//printf("JOB2: %s\n", work);
		//printf("DIFF: %s\n", diff);

		time(&start_t); // measure starting time
		for (int i = 0; i < (100 * atoi(diff)) + 1; i++) {
			char str_to_hash[128] = "";

			strcat(str_to_hash, job);

			itoa(i, ducos1_result_string);
			strcat(str_to_hash, ducos1_result_string);

			//printf("String to hash: %s\n", str_to_hash);

			unsigned char temp[SHA_DIGEST_LENGTH];
			char buf[SHA_DIGEST_LENGTH * 2];
			memset(buf, 0x0, SHA_DIGEST_LENGTH * 2);
			memset(temp, 0x0, SHA_DIGEST_LENGTH);
			SHA1(buf, str_to_hash, strlen(str_to_hash));
			long iZ = 0;
			for (iZ = 0; iZ < SHA_DIGEST_LENGTH; iZ++)
				sprintf((char*) & (buf[iZ * 2]), "%02x", temp[iZ]);

			//printf("Hashed res    : %s\n", buf);
			//printf("Expected res  : %s\n\n", work);

			if (strcmp(work, buf) == 0) {
				//printf("Found share! %s\n", ducos1_result_string);

				/* Calculate hashrate */
				time(&end_t);
				diff_t = difftime(end_t, start_t);
				hashrate = (atoi(ducos1_result_string) / diff_t) / 1000;

				if (send(socket_desc, ducos1_result_string, strlen(ducos1_result_string), 0) < 0) { //send result
					printf("Error: Couldn't send result\n");
					return 1;
				}

				char feedback[6] = "";
				if (recv(socket_desc, feedback, 6, 0) < 0) { // receive feedback
					printf("Error: Feedback couldn't be received\n");
					return 1;
				}

				//printf("Feedback: %s\n", feedback);
				if (strcmp("GOOD\n", feedback) == 0 || strcmp("BLOCK\n", feedback) == 0) {
					accepted_shares++;
					printf("Accepted share #%i (%s) %i kH/s\n", accepted_shares, ducos1_result_string, hashrate);

				} else if (strcmp("INVU\n", feedback) == 0) {
					printf("Error: Incorrect username\n");
					return 1;

				} else {
					rejected_shares++;
					printf("Rejected share #%i (%s) %i kH/s\n", rejected_shares, ducos1_result_string, hashrate);
				}
				break;
			}
		}
	}
	return 0;
}

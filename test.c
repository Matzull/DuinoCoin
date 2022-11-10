#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha1.h"

int main (int argc, char **argv) {
	
	char* input = "hola";
	char* output = malloc(20);
	char* digest = malloc(40);

	SHA1(output, input, strlen(input));

	
	for (int i = 0; i < 20; i += 1) {
		printf("%x", output[i] &0xff);
	}
	sprintf(digest ,"%x", output[i] &0xff);
	putchar('\n');

	//printf("%s", output);

	return 0;
}
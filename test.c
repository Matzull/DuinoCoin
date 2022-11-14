#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha1.h"

void toHex(char* input, char* digest)
{
	for (int i = 0; i < 20; i += 1) {
		snprintf(&digest[2 * i], 3, "%02x", input[i]&0xff);
	}
}

int main (int argc, char **argv) {
	
	char* input = "hola";
	char* output = malloc(20);
	char* digest = malloc(40);

	SHA1(output, input, strlen(input));
	

	

	return 0;
}
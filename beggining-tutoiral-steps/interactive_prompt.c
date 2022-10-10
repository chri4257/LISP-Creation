#include <stdio.h>

/* Interactive prompt that repeatedly writes a message and then waits for input */


/* Declare a buffer for the user input */
static char input[2048];

int main(int argc, char** argv) {
	puts("CHISP Version 0.0.0.0.1");
	puts("Press Crtl+c to Exit\n");
	
	/* Never ending loop */
	while (1) {
		
		/*	Output our prompt */
		fputs("chisp> ", stdout);
		
		/* Read a line of input (of max size 2048) */
		fgets(input, 2048, stdin);
		
		/* Echo input to user */
		printf("You are a %s", input);
	}
	
	return 0;
}

	
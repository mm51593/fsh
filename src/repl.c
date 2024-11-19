#include "src/repl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROMPT "fsh> "
#define LINE_LENGTH 256

void print_prompt() {
	printf(PROMPT);	
}

void read_line(char *line) {
	if (fgets(line, LINE_LENGTH, stdin) == NULL) {
		line[0] = '\0';
	}
}

int eval(char *line) {
	printf("%lu\n", strlen(line));
	return EXIT_SUCCESS;
}

void loop() {
	char line[LINE_LENGTH];

	do {
		print_prompt();
		read_line(line);
		eval(line);

	} while(strlen(line) != 0);
}

#include "src/repl.h"
#include "src/eval.h"
#include "src/constants.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PROMPT "fsh"
#define LINE_LENGTH 256

void print_prompt() {
	char cwd[PATH_MAX];
	char prompt[] = PROMPT;
	getcwd(cwd, PATH_MAX);
	printf("%s %s > ", prompt, cwd);
}

void read_line(char *line) {
	line[0] = '\0';
	char *buff_ptr = fgets(line, LINE_LENGTH, stdin);
	if (buff_ptr == NULL) {
		if (!feof(stdin)) {
			line[0] = '\n';
			line[1] = '\0';
		}
		printf("\n");
	}
	clearerr(stdin);
}

void loop() {
	char line[LINE_LENGTH] = { 0 };
	unsigned long line_length;

	do {
		print_prompt();
		read_line(line);
		line_length = strlen(line);
		eval(line);

	} while(line_length != 0);
}

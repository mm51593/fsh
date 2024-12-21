#include "src/repl.h"
#include "src/eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslimits.h>
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
	if (fgets(line, LINE_LENGTH, stdin) == NULL) {
		line[0] = '\0';
	}
}

void loop() {
	char line[LINE_LENGTH];

	do {
		print_prompt();
		read_line(line);
		eval(line);

	} while(strlen(line) != 0);
}

#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "unistd.h"

#define INPUT_BUF_SIZE 20
#define CMD_CD_STR "cd"
#define CMD_EXIT_STR "exit"

enum command {
	CMD_CD,
	CMD_EXIT,
	CMD_EXEC
};

void parse_input(char *line, char **input_buf, unsigned *argc) {
	size_t line_idx = 0;
	size_t input_buf_idx = 0;
	
	while (line[line_idx] != '\0') {
		if ((line_idx == 0 || line[line_idx - 1] == '\0')
		    && !isspace(line[line_idx])) {
			input_buf[input_buf_idx++] = line + line_idx;
			argc++;
		} else if (isspace(line[line_idx])) {
			line[line_idx] = '\0';
		}

		line_idx++;
	}

	input_buf[input_buf_idx] = NULL;

	return;
}

enum command determine_cmd(char **argv) {
	if (strcmp(argv[0], CMD_CD_STR) == 0) {
		return CMD_CD;
	} else if (strcmp(argv[0], CMD_EXIT_STR) == 0) {
		return CMD_EXIT;
	} else {
		return CMD_EXEC;
	}
}

int eval_cd(char **argv) {
	int retval = chdir(argv[1]);
	return retval;
}

int eval_exit(char **argv) {
	exit(EXIT_SUCCESS);
}

int eval_exec(char **argv) {
	int pid = fork();
	if (pid == 0) {
		execve(argv[0], argv, NULL);
	}

	wait(NULL);
	return 0;
}

int eval(char *line) {
	char *argv[INPUT_BUF_SIZE];
	unsigned argc = 0;

	parse_input(line, argv, &argc);
	int retval;

	switch (determine_cmd(argv)) {
        case CMD_CD:
		retval = eval_cd(argv);
		break;
        case CMD_EXIT:
		retval = eval_exit(argv);
		break;
        case CMD_EXEC:
		retval = eval_exec(argv);
		break;
        }

	if (retval != 0) {
		perror(argv[0]);
	}

	return retval;
}

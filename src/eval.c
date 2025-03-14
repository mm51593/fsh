#include "src/constants.h"

#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define INPUT_BUF_SIZE 20
#define CMD_CD_STR "cd"
#define CMD_EXIT_STR "exit"
#define PATH_SEPARATOR ':'

extern char **environ;

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
			(*argc)++;
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
	if (retval != 0) {
		perror(argv[0]);
	}
	return retval;
}

int eval_exit(char **argv) {
	exit(EXIT_SUCCESS);
}

_Bool check_if_exists(const char *name, char *buffer, int buffer_size, int buffer_idx) {
	buffer[buffer_idx] = '\0';
	strlcat(buffer, "/", buffer_size);
	strlcat(buffer, name, buffer_size);

	return access(buffer, F_OK) == 0;
}

const char *resolve_from_path(const char *name, char *buffer, int buffer_size) {
	const char *path = getenv("PATH");
	if (path == NULL) return name;

	int buffer_idx = 0;

	for (int i = 0; path[i] != '\0'; i++) {
		if (path[i] == PATH_SEPARATOR) {
			if (check_if_exists(name, buffer, buffer_size, buffer_idx)) {
				return buffer;
			}
			buffer_idx = 0;	
		} else {
			buffer[buffer_idx++] = path[i];
		}
	}

	return name;
}

const char *resolve_name(const char *name, char *buffer, int buffer_size) {
	if (name[0] != '/' && 
	   (name[0] != '.' || name[1] != '/')) {
		return resolve_from_path(name, buffer, PATH_MAX);
	}

	return name;
}

int spawn_subprocess(char **argv) {
	int pid = fork();
	char exe_path[PATH_MAX];

	switch (pid) {
	case -1:
		return EXIT_FAILURE;
		break;
	case 0: 
		resolve_name(argv[0], exe_path, PATH_MAX);
		if (execve(exe_path, argv, environ) != 0) {
			perror(argv[0]);
		}
		exit(EXIT_FAILURE);
	}

	return pid;
}

int eval_exec(char **argv) {
	int pid = spawn_subprocess(argv);
	int status;
	if (waitpid(pid, &status, 0) == -1) {
		return EXIT_FAILURE;
	}

	return WEXITSTATUS(status);
}

int eval(char *line_copy) {
	unsigned argc = 0;
	char *argv[INPUT_BUF_SIZE];

	parse_input(line_copy, argv, &argc);
	if (argc == 0) {
		return EXIT_SUCCESS;
	}

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

	return retval;
}

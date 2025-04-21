#include "src/process.h"
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
#define BG_RUN_FLAG "&"


enum command {
	CMD_CD,
	CMD_EXIT,
	CMD_EXEC
};

_Bool check_bg_run_flag(unsigned *argc, char *const *argv) {
	if (strcmp(argv[*argc - 1], BG_RUN_FLAG) == 0) {
		(*argc)--;
		return 1;
	}
	return 0;
}

void parse_input(char *line, char **input_buf, unsigned *argc, _Bool *bg_flag) {
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

	*bg_flag = *argc != 0 && check_bg_run_flag(argc, input_buf);

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

int spawn_subprocess(char *const *argv) {
	int pid = fork();

	switch (pid) {
	case -1:
		return EXIT_FAILURE;
		break;
	case 0: 
		init_subprocess(argv);
		exit(EXIT_FAILURE);
	}

	return pid;
}

void move_self_to_fg() {
	move_to_fg(getpgid(getpid()));
}

int eval_exec(char **argv, _Bool should_bg_run) {
	int pid = spawn_subprocess(argv);
	int status = wait_subprocess(pid);

	move_to_new_process_group(pid);
	int pgid = pid;
        if (!should_bg_run) {
		move_to_fg(pgid);
        }

        resume_subprocess(pid);

	if (!should_bg_run) {
		status = wait_subprocess(pid);
	}

	move_self_to_fg();
	return status;
}

int eval(char *line_copy) {
	unsigned argc = 0;
	char *argv[INPUT_BUF_SIZE];
	_Bool should_bg_run = 0;

	parse_input(line_copy, argv, &argc, &should_bg_run);
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
		retval = eval_exec(argv, should_bg_run);
		break;
        }

	return retval;
}

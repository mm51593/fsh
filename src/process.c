#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PATH_SEPARATOR ':'

extern char **environ;

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

void move_to_new_process_group() {
	setpgid(getpid(), 0);
}

void init_subprocess(char *const *argv) {
	char exe_path[PATH_MAX];
	resolve_name(argv[0], exe_path, PATH_MAX);
	move_to_new_process_group();
	if (execve(exe_path, argv, environ) != 0) {
		perror(argv[0]);
	}
}

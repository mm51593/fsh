#include <sys/types.h>

void move_to_new_process_group(pid_t pid);
int wait_subprocess(pid_t pid);
void resume_subprocess(pid_t pid);
void move_to_fg(pid_t pgid);
void init_subprocess(char *const *argv);

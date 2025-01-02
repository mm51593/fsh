#include "handler.h"

#include <signal.h>
#include <stdio.h>
#include <sys/signal.h>
#include <stdlib.h>

int set_signal_handler(int signal, void (*handler)(int)) {
	struct sigaction act;
	act.sa_handler = handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, signal);
	return sigaction(signal, &act, NULL);
}

void sigint_handler() {
}

int set_sigint_handler() {
	return set_signal_handler(SIGINT, sigint_handler);
}

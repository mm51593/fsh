#include "src/repl.h"
#include "handler.h"

int setup() {
	int retval = 0;
	retval |= set_sigint_handler();
	return retval;
}

int main(void) {
	setup();
	loop();
	return 0;
}

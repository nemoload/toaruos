/* vim: tabstop=4 shiftwidth=4 noexpandtab
 *
 * init
 *
 * Provides the standard boot routines and
 * calls the user session (compositor / terminal)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <syscall.h>

#define DEFAULT_HOSTNAME "toaru-test"

void set_console() {
	int _stdin  = open("/dev/null", O_RDONLY);
	int _stdout = open("/dev/ttyS0", O_WRONLY);
	int _stderr = open("/dev/ttyS0", O_WRONLY);

	if (_stdout < 0) {
		_stdout = open("/dev/null", O_WRONLY);
		_stderr = open("/dev/null", O_WRONLY);
	}
}

/* Set the hostname to whatever is in /etc/hostname */
void set_hostname() {
	FILE * _host_file = fopen("/etc/hostname", "r");
	if (!_host_file) {
		/* No /etc/hostname, use the default */
		syscall_sethostname(DEFAULT_HOSTNAME);
	} else {
		char buf[256];
		fgets(buf, 255, _host_file);
		if (buf[strlen(buf)-1] == '\n') {
			buf[strlen(buf)-1] = '\0';
		}
		syscall_sethostname(buf);
		setenv("HOST", buf, 1);
		fclose(_host_file);
	}
}

int start_options(char * args[]) {
	int pid = fork();
	if (!pid) {
		int i = execvp(args[0], args);
		exit(0);
	} else {
		return syscall_wait(pid);
	}
}

int main(int argc, char * argv[]) {
	/* stdin/out/err */
	set_console();
	/* Hostname */
	set_hostname();
	if (argc > 1) {
		char * args = NULL;
		if (argc > 2) {
			args = argv[2];
		}
		if (!strcmp(argv[1],"--single")) {
			return start_options((char *[]){"/bin/compositor","/bin/terminal","-Fl",args,NULL});
		} else if (!strcmp(argv[1], "--single-beta")) {
			return start_options((char *[]){"/bin/compositor-beta",NULL});
		} else if (!strcmp(argv[1], "--vga")) {
			return start_options((char *[]){"/bin/terminal-vga","-l",NULL});
		}
	}
	return start_options((char *[]){"/bin/compositor",NULL});
}

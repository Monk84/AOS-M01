
#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void)
{
	pid_t pid;
	int pipe_1[2];
	if (pipe(pipe_1) == -1) {
		perror("pipe");
		exit(1);
	}
	switch(pid=fork()) {
	case -1:
        	perror("fork");
        	exit(1);
	case 0: {
		close(pipe_1[1]);
		dup2(pipe_1[0], STDIN_FILENO);
		execlp("wc", "wc", "-l", NULL);
		//close(pipe_1[1]);
        	exit(0);
	}
	default: {
		close(pipe_1[0]);
		dup2(pipe_1[1], STDOUT_FILENO);
		execlp("ls", "ls", NULL);
		wait(NULL);
	}
	}
	return 0;
}

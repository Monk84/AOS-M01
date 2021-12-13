
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
	ssize_t message_len;
	char buf[256] = {0};
	if (pipe(pipe_1) == -1) {
		perror("pipe");
		exit(1);
	}
	switch(pid=fork()) {
	case -1:
        	perror("fork");
        	exit(1);
	case 0: {
		close(pipe_1[0]);
		dup2(pipe_1[1], STDOUT_FILENO);
		execlp("ls", "ls", NULL);
		close(pipe_1[1]);
        	exit(0);
	}
	default: {
		close(pipe_1[1]);
		while (1) {
			if ((message_len = read(pipe_1[0], buf, 255))) {
				if (message_len == -1)
					printf("Parent - read error\n");
				else {
					printf("Parent - message from parent: \"%s\"\n", buf);
					memset(buf, 0, 255);
				}
			}
		}
		close(pipe_1[0]);
		wait(NULL);
	}
	}
	return 0;
}

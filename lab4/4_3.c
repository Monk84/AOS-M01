
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
	int pipe_1[2], pipe_2[2];
	ssize_t message_len;
	char buf[256] = {0};
	if (pipe(pipe_1) == -1) {
		perror("pipe");
		exit(1);
	}
	if (pipe(pipe_2) == -1) {
		perror("pipe");
		exit(1);
	}
	switch(pid=fork()) {
	case -1:
        	perror("fork");
        	exit(1);
	case 0: {
		close(pipe_1[0]);
		close(pipe_2[1]);
		while (1) {
			if ((message_len = read(STDIN_FILENO, buf, 255))) {
				if (message_len == -1)
					printf("Child - read error\n");
				else {
					buf[message_len - 1] = 0;
					write(pipe_1[1], buf, message_len);
					memset(buf, 0, 255);
				}
			}
			if ((message_len = read(pipe_2[0], buf, 255))) {
				if (message_len == -1)
					printf("Child - read error\n");
				else {
					printf("Child - message from parent: \"%s\"\n", buf);
					memset(buf, 0, 255);
				}
			}
		}
		close(pipe_1[1]);
		close(pipe_2[0]);
        	exit(0);
	}
	default: {
		close(pipe_1[1]);
		close(pipe_2[0]);
		while (1) {
			if ((message_len = read(STDIN_FILENO, buf, 255))) {
				if (message_len == -1)
					printf("Parent - read error\n");
				else {
					buf[message_len - 1] = 0;
					write(pipe_2[1], buf, message_len);
					memset(buf, 0, 255);
				}
			}
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
		close(pipe_2[1]);
		wait(NULL);
	}
	}
	return 0;
}

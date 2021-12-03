#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void child_hndlr(int signum)
{
	int ret;
	wait(&ret);
	printf("SIGHANDLER: Child returned %d\n", ret);
	(void)signum;
}

int main(void)
{
	pid_t pids[10];
	for (int i = 0; i < 2; ++i) {
		if ((pids[i] = fork()) < 0) {
			perror("fork");
			exit(1);
		} else if (pids[i] == 0) {
			printf("Child %d born!\n", i);
			for (int j = 0; j < 10000; ++j);
			printf("Child %d dies!\n", i);
        		exit(0);
		}
	}	
	signal(SIGCHLD, child_hndlr);
	while (1) {
		printf("Parent loop\n");
		sleep(1);
	}
	return 0;
}

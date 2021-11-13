#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
	pid_t pid;
	int retst;
	switch(pid=fork()) {
	case -1:
        	perror("fork");
        	exit(1); 
	case 0:
        	printf("CHILD: My PID -- %d\n", getpid());
        	printf("CHILD: My PGID -- %d\n", getpgrp());
        	printf("CHILD: My PPID -- %d\n", getppid());
        	exit(0);
	default:
        	printf("PARENT: Мy PID -- %d\n", getpid());
        	printf("PARENT: My PGID -- %d\n", getpgrp());
        	printf("PARENT: My PPID -- %d\n", getppid());
		printf("PARENT: CHILD has PID %d\n", wait(&retst));
		printf("PARENT: CHILD return status %d\n", retst);
	}
	return 0;
}

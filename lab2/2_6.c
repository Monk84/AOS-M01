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
        	//sleep(3);
		pause();
	default:
        	printf("PARENT: Мy PID -- %d\n", getpid());
        	printf("PARENT: My PGID -- %d\n", getpgrp());
        	printf("PARENT: My PPID -- %d\n", getppid());
		pause();
		//exit(0);
	}
	return 0;
}

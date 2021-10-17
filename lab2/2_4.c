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
        	printf("CHILD: My PPID -- %d\n", getppid());
		for (int i = 0; i < 1000000; ++i);
        	printf("CHILD: My (new) PPID -- %d\n", getppid());
        	exit(0);
	default:
        	printf("PARENT: Мy PID -- %d\n", getpid());
		exit(0);
	}
	return 0;
}

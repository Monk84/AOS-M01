#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
	pid_t pid;
	switch(pid=fork()) {
	case -1:
        	perror("fork");
        	exit(1); 
	case 0:
        	printf("CHILD: My PID -- %d\n", getpid());
        	exit(0);
	default:
        	printf("PARENT: Мy PID -- %d\n", getpid());
	}
	return 0;
}

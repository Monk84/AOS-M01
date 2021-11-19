#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void three_four()
{
	pid_t pid;
	int retst;
	switch(pid=fork()) {
	case -1:
        	perror("fork");
        	exit(1); 
	case 0: { // child
		pause();	
		break;
	}
	default:
		wait(&retst);
		printf("CHILD exit status %d\n", retst);
	}
}

void three_five()
{
	pid_t pid, wait_ret;
	int retst;
	switch(pid=fork()) {
	case -1:
        	perror("fork");
        	exit(1); 
	case 0: { // child
		pause();	
		break;
	}
	default:
		printf("Sending to child signal %d\n", SIGUSR1);
		kill(pid, SIGUSR1);
		wait_ret = wait(&retst);
		if (WIFSIGNALED(retst))
			printf("Child was stopped by signal %d\n", WTERMSIG(retst));

		printf("wait() returned %d\n", wait_ret);
		printf("CHILD exit status %d\n", retst);
	}
}

int main(void)
{
	//three_four();
	three_five();
	return 0;
}

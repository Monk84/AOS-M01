#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void chldhdlr(int signum)
{
	printf("Silently ignoring signal %d\n", signum);
}

void work(int intercept_alarm)
{
	pid_t pid, wait_ret;
	int retst;
	switch(pid=fork()) {
	case -1:
        	perror("fork");
        	exit(1); 
	case 0: { // child
		alarm(1);
		if (intercept_alarm) {
			printf("intercepting alarm\n");
			signal(SIGALRM, chldhdlr);
		}
		for (int i = 0; i < 100; ++i) {
			for (int j = 0; j < 3000; ++j)
				for (int k = 0; k < 3000; ++k);
			printf("iteration %d\n", i);
		}
		break;
	}
	default:
		//printf("Sending to child signal %d\n", SIGALRM);
		//alarm(1);
		wait_ret = wait(&retst);
		if (WIFSIGNALED(retst))
			printf("Child was stopped by signal %d\n", WTERMSIG(retst));

		printf("CHILD exit status %d\n", retst);
		printf("wait() returned %d\n", wait_ret);
		printf("ERRNO: %d\n", errno);
		//printf("In sys err list %d means: %s", retst, strerror(retst));
	}
}

int main(int argc, char *argv[])
{
	if (argc > 1)
		work(1);
	else
		work(0);
	(void)argv;
	return 0;
}

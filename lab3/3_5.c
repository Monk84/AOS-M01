#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void)
{
	pid_t pid, wait_ret;
	int retst;
	switch(pid=fork()) {
	case -1:
        	perror("fork");
        	exit(1); 
	case 0: { // child
		for (int i = 0; i < 100; ++i) {
			for (int j = 0; j < 3000; ++j)
				for (int k = 0; k < 3000; ++k);
			printf("iteration %d\n", i);
		}
		break;
	}
	default:
		printf("Sending to child signal %d\n", SIGUSR1);
		kill(pid, SIGUSR1);
		wait_ret = wait(&retst);
		if (WIFSIGNALED(retst))
			printf("Child was stopped by signal %d\n", WTERMSIG(retst));

		printf("CHILD exit status %d\n", retst);
		printf("wait() returned %d\n", wait_ret);
		printf("ERRNO: %d\n", errno);
		//printf("In sys err list %d means: %s", retst, strerror(retst));
	}
	return 0;
}

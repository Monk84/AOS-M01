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
	case 0: { // child
		for (int i = 0; i < 100; ++i) {
			for (int j = 0; j < 3000; ++j)
				for (int k = 0; k < 3000; ++k);
			printf("iteration %d\n", i);
		}
		break;
	}
	default:
		wait(&retst);
		printf("CHILD exit status %d\n", retst);
	}
	return 0;
}

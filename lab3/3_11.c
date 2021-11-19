#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void suhndlr(int signum)
{
	printf("Skipped signal ");
	if (SIGUSR1 == signum)
		printf("SIGUSR1\n");
	else
		printf("SIGUSR2\n");
}

void do_work(int iterations)
{
	for (int i = 1; i < iterations; ++i)
		sleep(1);
}

int main(void)
{
	pid_t pid;
	sigset_t to_block, empty;
	sigemptyset(&to_block);
	sigemptyset(&empty);
	sigaddset(&to_block, SIGUSR1);	
	sigaddset(&to_block, SIGUSR1);	
	sigprocmask(SIG_BLOCK, &to_block, NULL);
	switch(pid=fork()) {
	case -1:
        	perror("fork");
        	exit(1); 
	case 0: { // child
		signal(SIGUSR1, suhndlr);
		printf("CHILD STARTS 1\n");
		do_work(4);
		printf("CHILD ENDS 1\n");
		kill(getppid(), SIGUSR2);
		sigemptyset(&empty);
		sigsuspend(&empty);
		sigprocmask(SIG_BLOCK, &to_block, NULL);
		printf("CHILD STARTS 2\n");
		do_work(2);
		printf("CHILD ENDS 2\n");
		kill(getppid(), SIGUSR2);
		sigemptyset(&empty);
		sigsuspend(&empty);
		exit(0);
		}
	default: {
		signal(SIGUSR2, suhndlr);
		printf("PARENT STARTS 1\n");
		do_work(2);
		printf("PARENT ENDS 1\n");
		kill(pid, SIGUSR1);
		sigemptyset(&empty);
		sigsuspend(&empty);
		sigprocmask(SIG_BLOCK, &to_block, NULL);
		printf("PARENT STARTS 2\n");
		do_work(4);
		printf("PARENT ENDS 2\n");
		kill(pid, SIGUSR1);
		sigemptyset(&empty);
		sigsuspend(&empty);
		}
	}
	
	return 0;
}

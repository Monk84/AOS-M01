#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

void chldhdlr(int signum)
{
	printf("Silently ignoring signal %d\n", signum);
}

int main()
{
	sigset_t signls;
	sigfillset(&signls);
	sigdelset(&signls, SIGUSR1);
	signal(SIGUSR1, chldhdlr);
	printf("Blocking all signals except SIGINT (SIGSTOP and SIGKILL too)\n");
	sigprocmask(SIG_BLOCK, &signls, NULL);
	sleep(10);
	printf("Unblocking\n");
	sigprocmask(SIG_UNBLOCK, &signls, NULL);
	return 0;
}


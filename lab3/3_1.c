#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void sighandler(int signum)
{
	printf("this is non-default handler\n");
	signal(signum, SIG_DFL);
}

int main()
{
	signal(SIGINT, sighandler);
	for (int i = 0;; ++i) {
		printf("program loop, iteration %d\n", i);
		sleep(1);
	}
	return 0;
}

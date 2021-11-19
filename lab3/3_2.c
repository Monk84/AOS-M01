#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void hndlr(int signum)
{
	printf("this is non-default handler\n");
	struct sigaction def;
	sigemptyset(&def.sa_mask);
	def.sa_flags = 0;
	def.sa_handler = SIG_DFL;
	sigaction(signum, &def, NULL);
}

int main(void)
{
	struct sigaction new;
	new.sa_handler = hndlr;
	new.sa_flags = 0;
	sigemptyset(&new.sa_mask);
	sigaction(SIGINT, &new, NULL);
	for (int i = 0;; ++i) {
		printf("program loop, iteration %d\n", i);
		sleep(1);
	}
	return 0;
}

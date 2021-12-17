#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
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

struct msgbuf {
    long mtype;
    char mtext[1];
};


int main(void)
{
	int q;
	key_t key = ftok(".", 'a'); // key for server mqueue
	if ((q = msgget(key, IPC_CREAT | 0600)) == -1) {
		printf("error while creating queue! errno %d\n", errno);
		exit(1);
	}
	ssize_t msgsize;
	struct msgbuf msg;
	pid_t pid;
	switch(pid=fork()) {
	case -1:
        	perror("fork");
        	exit(1); 
	case 0: { // child
		printf("CHILD STARTS 1\n");
		do_work(6);
		printf("CHILD ENDS 1\n");
		msg.mtype = 1;
		if (msgsnd(q, &msg, sizeof(msg.mtext), IPC_NOWAIT) == -1)
			printf("error while sending message to parent\n");
		while (1) {
			if ((msgsize = msgrcv(q, &msg, sizeof(msg.mtext), 2,
											IPC_NOWAIT | MSG_NOERROR)) == -1) {
				if (errno == ENOMSG)
					continue;
				else {
					printf("error while waiting message from parent!");
					exit(1);
				}
			}
			printf("child syncronized!\n");
			break;
		}
		printf("CHILD STARTS 2\n");
		do_work(2);
		printf("CHILD ENDS 2\n");
		msg.mtype = 1;
		if (msgsnd(q, &msg, sizeof(msg.mtext), IPC_NOWAIT) == -1)
			printf("error while sending message to parent\n");
		while (1) {
			if ((msgsize = msgrcv(q, &msg, sizeof(msg.mtext), 2,
											IPC_NOWAIT | MSG_NOERROR)) == -1) {
				if (errno == ENOMSG)
					continue;
				else {
					printf("error while waiting message from parent!");
					exit(1);
				}
			}
			printf("child syncronized!\n");
			break;
		}
		exit(0);
		}
	default: {
		printf("PARENT STARTS 1\n");
		do_work(2);
		printf("PARENT ENDS 1\n");
		msg.mtype = 2;
		if (msgsnd(q, &msg, sizeof(msg.mtext), IPC_NOWAIT) == -1)
			printf("error while sending message to child\n");
		while (1) {
			if ((msgsize = msgrcv(q, &msg, sizeof(msg.mtext), 1,
											IPC_NOWAIT | MSG_NOERROR)) == -1) {
				if (errno == ENOMSG)
					continue;
				else {
					printf("error while waiting message from child!");
					exit(1);
				}
			}
			printf("parent syncronized!\n");
			break;
		}
		printf("PARENT STARTS 2\n");
		do_work(6);
		printf("PARENT ENDS 2\n");
		msg.mtype = 2;
		if (msgsnd(q, &msg, sizeof(msg.mtext), IPC_NOWAIT) == -1)
			printf("error while sending message to child\n");
		while (1) {
			if ((msgsize = msgrcv(q, &msg, sizeof(msg.mtext), 1,
											IPC_NOWAIT | MSG_NOERROR)) == -1) {
				if (errno == ENOMSG)
					continue;
				else {
					printf("error while waiting message from child!");
					exit(1);
				}
			}
			printf("parent syncronized!\n");
			break;
		}
		}
	}
	
	return 0;
}

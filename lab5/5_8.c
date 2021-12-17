#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int to_live;

void sigint_handler(int signum)
{
	(void)signum;
	to_live = 0;
}

struct msgbuf {
    long mtype;
    char mtext[60];
};

struct process_payload {
	pid_t pid;
};

void server_handling()
{
	pid_t client;
	int cq;
	key_t key = ftok(".", 'a'); // key for server mqueue
	int sq;
	if ((sq = msgget(key, IPC_CREAT | 0600)) == -1) {
		printf("error while creating server queue! errno %d\n", errno);
		exit(1);
	}
	ssize_t msgsize;
	struct msgbuf msg;
	char message[50] = "Hello to process ";
	int init_msg_len = strlen(message);
	signal(SIGINT, sigint_handler);
	to_live = 1;
	while (to_live) {
		sleep(1);
		if ((msgsize = msgrcv(sq, &msg, sizeof(msg.mtext), 0, 
											IPC_NOWAIT | MSG_NOERROR)) == -1)
			continue;
		struct process_payload *p = (struct process_payload *)&(msg.mtext[0]);
		client = p->pid;
		if ((cq = msgget(client, 0600)) == -1) {
			printf("error while accessing client queue! errno %d\n", errno);
			continue;
		}
		message[init_msg_len + sprintf(message + init_msg_len, "%d", client)] = 0;
		msg.mtype = 1;
		strcpy(msg.mtext, message);
        if (msgsnd(cq, &msg, sizeof(msg.mtext), IPC_NOWAIT) == -1) {
			printf("error while sending message to process %d\n", client);
		}
		message[init_msg_len] = 0;
	}
    msgctl(sq, IPC_RMID, NULL);
}

void client_handling()
{
	pid_t client = getpid();
	int cq;
	key_t key = ftok(".", 'a'); // key for server mqueue
	int sq;
	if ((sq = msgget(key, IPC_CREAT | 0600)) == -1) {
		printf("error while accessing server queue! errno %d\n", errno);
		exit(1);
	}
	if ((cq = msgget(client, IPC_CREAT | 0600)) == -1) {
		printf("error while creating client queue! errno %d\n", errno);
		exit(1);
	}
	ssize_t msgsize;
	struct msgbuf msg;
	struct process_payload p;
	p.pid = client;
	while (1) {
		sleep(1);
		msg.mtype = 1;
		memcpy(msg.mtext, &p, sizeof(p));
        if (msgsnd(sq, &msg, sizeof(msg.mtext), IPC_NOWAIT) == -1) {
			if (errno == EIDRM || errno == EINVAL) {
				printf("Server closed his mq\n");
				break;
			}
			printf("error while sending message to server\n");
		}
		if ((msgsize = msgrcv(cq, &msg, sizeof(msg.mtext), 0, 
											IPC_NOWAIT | MSG_NOERROR)) == -1) {
			if (errno = EAGAIN)
				continue;
			printf("error while getting message from server!\n");
			break;
		}
		printf("Got from server: %s\n", msg.mtext);
	}
    msgctl(cq, IPC_RMID, NULL);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s type (client/server)\n", argv[0]);
        exit(1);
    }
	if (!strcmp("server", argv[1]))
		server_handling();
	else if (!strcmp("client", argv[1]))
		client_handling();
    return 0;
}


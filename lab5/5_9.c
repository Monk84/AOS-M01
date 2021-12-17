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

void server_handling()
{
	pid_t client;
	int cq;
	int sq;
	key_t server_key = ftok(".", 'a'); // key for server mqueue
	key_t client_key = ftok(".", 'b'); // key for clients queue
	if ((sq = msgget(server_key, IPC_CREAT | 0600)) == -1) {
		printf("error while creating server queue! errno %d\n", errno);
		exit(1);
	}
	if ((cq = msgget(client_key, IPC_CREAT | 0600)) == -1) {
		printf("error while creating client queue! errno %d\n", errno);
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
		client = msg.mtype;
		printf("Got message \"%s\" from process %ld\n", msg.mtext, msg.mtype);
		message[init_msg_len + sprintf(message + init_msg_len, "%d", client)] = 0;
		msg.mtype = client;
		strcpy(msg.mtext, message);
        if (msgsnd(cq, &msg, sizeof(msg.mtext), IPC_NOWAIT) == -1) {
			printf("error while sending message to process %d\n", client);
		}
		message[init_msg_len] = 0;
	}
    msgctl(sq, IPC_RMID, NULL);
    msgctl(cq, IPC_RMID, NULL);
}

void client_handling()
{
	pid_t client = getpid();
	int cq;
	int sq;
	key_t server_key = ftok(".", 'a'); // key for server mqueue
	key_t client_key = ftok(".", 'b'); // key for clients queue
	if ((sq = msgget(server_key, IPC_CREAT | 0600)) == -1) {
		printf("error while creating server queue! errno %d\n", errno);
		exit(1);
	}
	if ((cq = msgget(client_key, IPC_CREAT | 0600)) == -1) {
		printf("error while creating client queue! errno %d\n", errno);
		exit(1);
	}
	ssize_t msgsize;
	char *message = "Hi!";
	struct msgbuf msg = {client, "Hi!"};
	while (1) {
		sleep(1);
		msg.mtype = client;
        if (msgsnd(sq, &msg, sizeof(msg.mtext), IPC_NOWAIT) == -1) {
			if (errno == EIDRM || errno == EINVAL) {
				printf("Server closed his mq\n");
				break;
			}
			printf("error while sending message to server\n");
		}
		if ((msgsize = msgrcv(cq, &msg, sizeof(msg.mtext), client, 
											IPC_NOWAIT | MSG_NOERROR)) == -1) {
			if (errno == EIDRM || errno == EINVAL) {
				printf("Server closed his mq\n");
				break;
			}
			if (errno = EAGAIN)
				continue;
			printf("error while getting message from server!\n");
			break;
		}
		printf("Got from server: %s\n", msg.mtext);
		msg.mtext[sprintf(msg.mtext, "%s", message)] = 0;
	}
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


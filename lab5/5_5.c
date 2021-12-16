#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>

struct msgbuf {
    long mtype;
    char mtext[30];
};

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s msqid\n", argv[0]);
        exit(1);
    }
    int msqid = atoi(argv[1]);
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        switch (errno) {
        case EACCES:
            printf("Error: access denied!\n");
            break;
        case EINVAL:
            printf("Error: invalid msqid!\n");
            break;
        case EPERM:
            printf("Error: not enough permissions!\n");
            break;
        }
        printf("msgctl returned -1, errno %d\n", errno);
    } else
        printf("successfully removed msq with msqid = %d\n", msqid);
    return 0;
}


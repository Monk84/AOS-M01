#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>

struct msgbuf {
    long mtype;
    char mtext[200];
};

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Usage: %s msqid mtype\n", argv[0]);
        exit(1);
    }
    int msqid = atoi(argv[1]);
    long mtype = atoi(argv[2]);
    struct msgbuf mymsg;
    ssize_t msgsize;
    while(1) {
        if ((msgsize = msgrcv(msqid, &mymsg, sizeof(mymsg.mtext), mtype, 0)) == 
                                                                        -1) {
            printf("msgrcv failure, errno %d\n", errno);
            exit(1);
        } else
            printf("succesfully got message \"%s\" of len %ld of type %ld "
                    "from the message queue\n", mymsg.mtext, msgsize, mymsg.mtype);
    }
    return 0;
}


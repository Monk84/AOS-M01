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

int main(void)
{
    int msqid;
    if ((msqid = msgget(0, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        printf("msgget returned -1, errno %d\n", errno);
        exit(1);
    } else
        printf("msgget returned %d\n", msqid);
    struct msqid_ds buf;
    if (msgctl(msqid, IPC_STAT, &buf) == -1) {
        printf("msgctl returned -1, errno %d\n", errno);
        exit(1);
    }
    printf("msg_perm: uid %d, gid %d, mode %o\n", buf.msg_perm.uid, 
                                buf.msg_perm.gid, buf.msg_perm.mode);
    printf("msg_stime: %ld\n", buf.msg_stime);
    printf("msg_rtime: %ld\n", buf.msg_rtime);
    printf("msg_ctime: %ld\n", buf.msg_ctime);
    printf("msg_qnum: %ld\n", buf.msg_qnum);
    printf("msg_lspid: %d\n", buf.msg_lspid);
    printf("msg_lrpid: %d\n", buf.msg_lrpid);
    printf("msg_qbytes: %ld\n", buf.msg_qbytes);
    struct msgbuf mymsg;
    mymsg.mtext[sprintf(mymsg.mtext, "Hello 0!")] = 0;
    for (int i = 1; i < 5; ++i) {
        mymsg.mtype = (long)i;
        mymsg.mtext[6] = i + 48;
        if (msgsnd(msqid, &mymsg, sizeof(mymsg.mtext), IPC_NOWAIT) == -1) {
            printf("msgsnd failure\n");
            exit(1);
        } else {
            printf("succesfully put message \"%s\" of type %ld into the "
                                "message queue\n", mymsg.mtext, mymsg.mtype);
        }
    }
    return 0;
}


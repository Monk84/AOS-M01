
#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char *argv[])
{
	pid_t pids[2];
	int fifofile;
	struct flock wrlocker, unlocker, rlocker;
	wrlocker.l_type = F_WRLCK;
	wrlocker.l_whence = SEEK_SET;
	wrlocker.l_start = 0;
	wrlocker.l_len = 0;
	wrlocker.l_pid = 0;
	unlocker.l_type = F_UNLCK;
	unlocker.l_whence = SEEK_SET;
	unlocker.l_start = 0;
	unlocker.l_len = 0;
	unlocker.l_pid = 0;
	rlocker.l_type = F_RDLCK;
	rlocker.l_whence = SEEK_SET;
	rlocker.l_start = 0;
	rlocker.l_len = 0;
	rlocker.l_pid = 0;
	ssize_t message_len;
	if (argc < 3) {
		printf("Usage: %s fname locksetter(write|read)\n", argv[0]);
		exit(1);
	}
	int typelockread;
	if (!strcmp(argv[2], "read"))
		typelockread = 1;
	else 
		typelockread = 0;
	const char *fname = argv[1];
	char buf[256] = {0};
	for (int i = 0; i < 2; ++i)
		switch (pids[i]=fork()) {
		case -1:
        		perror("fork");
        		exit(1);
		case 0: {
			if ((fifofile = open(fname, O_RDWR|O_CREAT)) == -1) {
				perror("open fifofile!\n");
				exit(1);
			}
			while(1) {
				if (i == 1) {
					if (typelockread && fcntl(fifofile, F_SETLK, 
										&rlocker) == -1) {
						printf("fcntl - cannot set rlock, so skip!\n");
						continue;
					}
					if ((message_len = read(fifofile, buf, 255)) == -1) {
						printf("Child - read() error!\n");
						printf("Errno %d\n", errno);
						continue;
					} else if (!message_len) {
						printf("0");
						continue;
					} else {
						buf[message_len] = 0;
						printf("Child %d - file contents: %s\n", i, buf);
					}
					if (typelockread && fcntl(fifofile, F_SETLK, 
										&unlocker) == -1)
						printf("fcntl - cannot set unlock!\n");
				}
				else if (!i) { 
					const char *string = "hello";
					if (!typelockread && fcntl(fifofile, F_SETLK, 
										&wrlocker) == -1) {
						printf("fcntl - cannot set wrlock, so continue!\n");
						continue;
					}
					if ((message_len = write(fifofile, string, strlen(string) + 1)) == -1) {
						printf("Child - write() error!\n");
						printf("Errno %d\n", errno);
						continue;
					} else {
						buf[message_len] = 0;
						printf("Child %d - sending \"%s\"\n", i, string);
					}
					if (!typelockread && fcntl(fifofile, F_SETLK, 
										&unlocker) == -1)
						printf("fcntl - cannot set unlock!\n");
				}
			}
			close(fifofile);
	        	exit(0);
		}
		}
	wait(NULL);
	wait(NULL);
	return 0;
}

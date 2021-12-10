
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
	ssize_t message_len;
	if (argc < 2) {
		printf("Usage: %s fname\n", argv[0]);
		exit(1);
	}
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
				}
				else if (!i) { 
					const char *string = "hello";
					if ((message_len = write(fifofile, string, strlen(string) + 1)) == -1) {
						printf("Child - write() error!\n");
						printf("Errno %d\n", errno);
						continue;
					} else {
						buf[message_len] = 0;
						printf("Child %d - sending \"%s\"\n", i, string);
					}
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

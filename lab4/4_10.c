
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

int main(void)
{
	pid_t pids[2];
	int piped[2];
	ssize_t message_len;
	char buf[256] = {0};
	if (pipe(piped) == -1) {
		perror("pipe");
		exit(1);
	}
	for (int i = 0; i < 2; ++i)
		switch (pids[i]=fork()) {
		case -1:
        		perror("fork");
        		exit(1);
		case 0: {
			while(1) {
				if (i == 1) {
				    close(piped[0]);
					if ((message_len = read(STDIN_FILENO, buf, 255)) == -1) {
						printf("Child - read() error!\n");
						printf("Errno %d\n", errno);
						continue;
					} else {
						buf[message_len] = 0;
						printf("Child %d - sending: %s\n", i, buf);
						if ((message_len = write(piped[1], buf, 
												 message_len)) == -1) {
								printf("Child %d - write() error!, "
														"errno %d\n", i, errno);
								continue;
						}
					}
				}
				else if (!i) { 
				    close(piped[1]);
					if ((message_len = read(piped[0], buf, 255)) == -1) {
						printf("Child - read() error!\n");
						printf("Errno %d\n", errno);
						continue;
					} else {
						buf[message_len] = 0;
						printf("Child %d - got: %s\n", i, buf);
						if ((message_len = write(STDOUT_FILENO, buf, 
												 message_len)) == -1) {
								printf("Child %d - write() error!, "
														"errno %d\n", i, errno);
								continue;
						}
					}
			    }
		    }
		close(piped[0]);
		close(piped[1]);
		exit(0);
		    }
		};
	close(piped[0]);
    close(piped[1]);
	wait(NULL);
	wait(NULL);
	return 0;
}

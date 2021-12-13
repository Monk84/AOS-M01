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
	struct flock wlocker, unlocker, rlocker, getlock;
	getlock.l_type = F_WRLCK;
	getlock.l_whence = SEEK_SET;
	getlock.l_start = 0;
	getlock.l_len = 0;
	getlock.l_pid = 0;
	wlocker.l_type = F_WRLCK;
	wlocker.l_whence = SEEK_SET;
	wlocker.l_start = 0;
	wlocker.l_len = 0;
	wlocker.l_pid = 0;
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
			if (i == 1) {
				unlocker.l_pid = getpid();
				rlocker.l_pid = getpid();
				//fcntl(STDOUT_FILENO, F_SETLKW, &unlocker);
				const char *fail_setlk = "cannot set lock on STDIN!\n",
					  	   *fail_read = "STDIN read error!\n",
						   *fail_write = "pipe write error!\n",
						   *fail_unlock = "cannot unlock STDIN!\n";
				close(piped[0]);
				for (int j = 0; j < 15; ++j) {    
                    if (fcntl(STDIN_FILENO, F_SETLKW, &rlocker) == -1) {
				    write(STDOUT_FILENO, fail_setlk, strlen(fail_setlk) + 1);
					//printf("cannot set lock on STDIN\n!");
							continue;
					}
					if ((message_len = read(STDIN_FILENO, buf, 255)) == -1) {
						write(STDOUT_FILENO, fail_read, strlen(fail_read) + 1);
						//printf("Child - read() error!\n");
						//printf("Errno %d\n", errno);
					} else {
						buf[message_len] = 0;
						//write(STDOUT_FILENO, , strlen(fail_read) + 1);
						printf("Child %d - sending: %s\n", i, buf);
						if ((message_len = write(piped[1], buf, 
												 message_len)) == -1) {
								write(STDOUT_FILENO, fail_write, 
														strlen(fail_write) + 1);
								//printf("Child %d - write() error!, "
								//						"errno %d\n", i, errno);
						}
					}
					if (fcntl(STDIN_FILENO, F_SETLKW, &unlocker) == -1)
						write(STDOUT_FILENO, fail_unlock, 
														strlen(fail_unlock) + 1);
						//	printf("cannot unlock STDIN!\n");
				}
            } else if (!i) { 
				const char *fail_setlk = "cannot set lock on STDOUT!\n",
					  	   *fail_read = "pipe read error!\n",
						   *fail_write = "STDOUT write error!\n",
						   *fail_unlock = "cannot unlock STDOUT!\n";
				unlocker.l_pid = getpid();
				wlocker.l_pid = getpid();
				getlock.l_pid = 0;
				close(piped[1]);
				for (int j = 0; j < 15; ++j) {
                    fcntl(STDOUT_FILENO, F_GETLK, &getlock);
					printf("LOCK ON STDOUT by process %d\n", getlock.l_pid);
				    printf("I AM %d\n", getpid());
					if (fcntl(STDOUT_FILENO, F_SETLK, &wlocker) == -1) {
							write(STDOUT_FILENO, fail_setlk, 
														strlen(fail_setlk) + 1);
							//printf("errno %d\n", errno);
							continue;
					}
					if ((message_len = read(piped[0], buf, 255)) == -1) {
						write(STDOUT_FILENO, fail_read, strlen(fail_read) + 1);
						//printf("Child - read() error!\n");
						//printf("Errno %d\n", errno);
					} else {
						buf[message_len] = 0;
						//printf("Child %d - got: %s\n", i, buf);
						if ((message_len = write(STDOUT_FILENO, buf, 
												 message_len)) == -1) {
								write(STDOUT_FILENO, fail_write, 
														strlen(fail_write) + 1);
								//printf("Child %d - write() error!, "
								//						"errno %d\n", i, errno);
						} else {
								const char *mess = " was read from pipe\n";
								write(STDOUT_FILENO, mess, strlen(mess) + 1);
						}
					}
					if (fcntl(STDOUT_FILENO, F_SETLKW, &unlocker) == -1)
							write(STDOUT_FILENO, fail_unlock, strlen(fail_unlock) + 1);
							//printf("cannot unlock STDOUT!\n");
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

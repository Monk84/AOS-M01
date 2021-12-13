
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
	struct flock wrlocker, unlocker, rlocker, lockgetter;
	lockgetter.l_type = F_WRLCK;
	lockgetter.l_whence = SEEK_SET;
	lockgetter.l_start = 0;
	lockgetter.l_len = 0;
	lockgetter.l_pid = 0;
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
	if (argc < 2) {
		printf("Usage: %s fname\n", argv[0]);
		exit(1);
	}
	const char *fname = argv[1];
	char buf[256] = {0};
	char outbuf[300] = {0};
	for (int i = 0; i < 2; ++i)
		switch (pids[i]=fork()) {
		case -1:
        		write(STDERR_FILENO, "fork", 5);
        		exit(1);
		case 0: {
			int mover = 0;
			if (i == 1) {
				if ((fifofile = open(fname, O_RDONLY|O_CREAT|O_TRUNC, 
																0777)) == -1) {
						mover = sprintf(outbuf, "open fifofile!\n");
						outbuf[mover] = 0;
						write(STDOUT_FILENO, outbuf, strlen(outbuf) + 1);
						exit(1);
				}
                for (int j = 0; j < 10; ++j) {
					//printf("I AM %d\n", getpid());
					rlocker.l_pid = getpid();
					unlocker.l_pid = getpid();
						if (fcntl(fifofile, F_SETLKW, &rlocker) == -1) {
							mover = sprintf(outbuf, 
									"fcntl - cannot set rlock, so skip!\n");
							outbuf[mover] = 0;
							write(STDOUT_FILENO, outbuf, strlen(outbuf) + 1);
							continue;
						}
						if ((message_len = read(fifofile, buf, 255)) == -1) {
							mover = sprintf(outbuf, "Child - read() error!\n");
							outbuf[mover] = 0;
							write(STDOUT_FILENO, outbuf, strlen(outbuf) + 1);
							//printf("Errno %d\n", errno);
						} else if (!message_len) {
						} else {
							buf[message_len] = 0;
							mover = sprintf(outbuf, "Child %d - file contents: "
															"%s\n", i, buf);
							outbuf[mover] = 0;
							write(STDOUT_FILENO, outbuf, strlen(outbuf) + 1);
						}
						if (fcntl(fifofile, F_SETLKW, 
											&unlocker) == -1) {
							mover = sprintf(outbuf, 
											"fcntl - cannot set unlock!\n");
							outbuf[mover] = 0;
							write(STDOUT_FILENO, outbuf, strlen(outbuf) + 1);
						}
						sleep(1);
				}
		    } else if (!i) {
				if ((fifofile = open(fname, O_WRONLY|O_CREAT|O_TRUNC, 
																0777)) == -1) {
						mover = sprintf(outbuf, "open fifofile!\n");
						outbuf[mover] = 0;
						write(STDOUT_FILENO, outbuf, strlen(outbuf) + 1);
						exit(1);
				}
                for (int j = 0; j < 10; ++j) {
				    const char *string = "hello";
				    wrlocker.l_pid = getpid();
				    unlocker.l_pid = getpid();
				    lockgetter.l_pid = getpid();
				    //if (fcntl(fifofile, F_GETLK, &lockgetter) == -1) {
					//	mover = sprintf(outbuf, "fcntl - cannot get current "
					//									"lock, so continue!\n");
					//	printf("errno %d\n", errno);
					//	outbuf[mover] = 0;
					//	write(STDOUT_FILENO, outbuf, strlen(outbuf) + 1);
					//	continue;
					//}
					//printf("fifofile locked by %d\n", lockgetter.l_pid);
					//printf("I AM %d\n", getpid());
					if (fcntl(fifofile, F_SETLKW, 
										&wrlocker) == -1) {
						mover = sprintf(outbuf, "fcntl - cannot set wrlock, "
														"so continue!\n");
						outbuf[mover] = 0;
						write(STDOUT_FILENO, outbuf, strlen(outbuf) + 1);
						continue;
					}
					if ((message_len = write(fifofile, 
												string, 
												strlen(string) + 1)) == -1) {
						mover = sprintf(outbuf, "Child - write() error!\n");
						outbuf[mover] = 0;
						write(STDOUT_FILENO, outbuf, strlen(outbuf) + 1);
					} else {
						buf[message_len] = 0;
						mover = sprintf(outbuf, "Child %d - sending \"%s\"\n", 
																i, string);
						outbuf[mover] = 0;
						write(STDOUT_FILENO, outbuf, strlen(outbuf) + 1);
					}
					if (fcntl(fifofile, F_SETLKW, 
										&unlocker) == -1) {
						mover = sprintf(outbuf, "fcntl - cannot set unlock!\n");
						outbuf[mover] = 0;
						write(STDOUT_FILENO, outbuf, strlen(outbuf) + 1);
					}
                    sleep(1);
				}
			}
		close(fifofile);
	    exit(0);
		}
	};
	wait(NULL);
	wait(NULL);
	return 0;
}

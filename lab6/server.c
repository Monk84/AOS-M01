/*******************************************************
 * var 6
 * server-client through datagram sockets
 * server-childs through shmem

             ( server )
             //  ||  \\
           (c1) (c2) (c3)                       
		   
*******************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>

#define SOCKET_PATH "/tmp/mysocket"
#define PID_PATH "/tmp/mypid"
#define CONFIG_PATH "/tmp/myconfig"

#define LOG_ERR "ERROR: "
#define LOG_INF "INFO: "
const enum {INFO = 0, ERROR = 1, CONTINUE = 2, } LOG_CONSTS;

int logfile;
int living;
int childs_num;
char logpath[101] = {};

void init_socket()
{
		
}

void logging(int logtype, char *message)
{
	switch(logtype) {
	case INFO:
		//if (write(logfd, LOG_INF, 6) == -1)
		//	return;
		write(logfile, LOG_INF, 6);
		break;
	case ERROR:
	default:
		write(logfile, LOG_INF, 6);
		break;
	};
	write(logfile, message, strlen(message));
	if (logtype != CONTINUE)
		write(logfile, "\n", 1);
	else
		write(logfile, " ", 1);
}

// closes current logfile and opens new
// binds new port to socket
int reload_resources()
{
	int tempfd;
	if (access(logpath, F_OK) == 0) {
		logging(ERROR, "file with given name exists! Exiting");
		return -1;
	}
	// TODO check port
	char temp[128];
	if ((tempfd = open(logpath, O_WRONLY | O_CREAT)) == -1) {
		temp[sprintf(temp, "while open() new logfile, errno %d", errno)] = 0;
		logging(ERROR, temp);
		return -1;
	}
	if (logfile != STDOUT_FILENO) { // don't print that on init_logging
		temp[sprintf(temp, "changing log file to \"%s\"", logpath)] = 0;
		logging(INFO, temp);
	}
	close(logfile);
	logfile = tempfd;
	return 1;
}

int load_config()
{
	int config;
	if ((config = open(CONFIG_PATH, O_RDONLY)) == -1) {
		logging(ERROR, "open() error while load_config!\n");
		return -errno;
	}
	char buf[105];
	ssize_t read_bytes;
	if ((read_bytes = read(config, buf, sizeof(buf) / sizeof(buf[0] - 1))) == 
																		-1) {
		logging(ERROR, "read() error while load_config!");
		return -errno;
	}
	buf[read_bytes] = 0;
	int new_childs_num;
	char new_logpath[101];
	if (sscanf(buf, "%d\n%100s", &new_childs_num, &new_logpath[0]) < 2) {
		logging(ERROR, "scanf() returned value < 2!");
		return 0;
	}
	if (childs_num == new_childs_num && !strcmp(logpath, new_logpath)) {
		logging(INFO, "parameters are the same, not reloading resources");
		return 2;
	}
	childs_num = new_childs_num;
	strcpy(logpath, new_logpath);
	return reload_resources();
}

void logging_init()
{
	logfile = STDOUT_FILENO;
	switch (load_config()) {
	case 0:
		printf("Invalid config!\n");
		exit(EINVAL);
	case 1:
		break;
	default:
		printf("error while acquiring resources!\n");
		exit(EAGAIN);
	};
}

void reload_config(int signum)
{
	(void)signum;
	if (load_config() != 1)
		logging(INFO, "parameters weren't updated");
}

void clean_up(int signum)
{
	(void)signum;
	pid_t childpid;
	int retst;
	char buf[200];
	for (int i = 0; i < childs_num; ++i) {
		childpid = wait(&retst);
		if (retst == -1) {
			buf[sprintf(buf, "waitpid returned -1, errno %d", errno)] = 0;
			logging(ERROR, buf);
		} else {
			buf[sprintf(buf, "child process %d returned %d", childpid, 
																	retst)] = 0;
			logging(INFO, buf);
		}
	}
	// TODO kill socket
	// TODO kill shmem
    logging(INFO, "daemon terminated");
	close(logfile);
	living = 0;
}

/************************************************************
 As recommended for SysV traditional daemonizing:
 - closing fd 0, 1, 2
 - fork from parent
 - in child, calling setsid() to detach from current TTY
 - in child, calling fork again (making child2)
 - exiting child to make 'init' parent for child2
 - connecting fd 0, 1, 2 of child2 to /dev/null
 - reset umask() to 0
 - change cwd to '/'
 - store PID of child2 in separate file
 - kill parent                                               
*************************************************************/
void daemonize()
{
	pid_t pid;

	switch (pid = fork()) {
	case -1:
		printf("First fork error!\n");
		exit(EXIT_FAILURE);
	case 0:
		break;
	default: // parent dies
		exit(EXIT_SUCCESS);
	}
	
	if (setsid() < 0) { // detach from current TTY
		printf("setsid() error, errno %d\n", errno);
		exit(EXIT_FAILURE);
	}
	
	switch(pid = fork()) {
	case -1:
		printf("Second fork error!\n");
		exit(EXIT_FAILURE);
	case 0:
		break;
	default: //child dies, only child2 alive
	    exit(EXIT_SUCCESS);
	}
	
	// handling special signals
	signal(SIGHUP, reload_config);
	signal(SIGTERM, clean_up);

	// reset file permissions
	umask(0);
	
	int pidfile;
	if ((pidfile = open(PID_PATH, O_WRONLY | O_CREAT | O_TRUNC)) == -1)
		logging(ERROR, "open() for writing PID returned -1");
	else {
		char temp[10];
		temp[sprintf(temp, "%d", getpid())] = 0;
		if (write(pidfile, temp, strlen(temp)) < (ssize_t)strlen(temp))
			logging(ERROR, "write() to pidfile returned value < len of buf");
	}
	close(pidfile);

	int devnull = open("/dev/null", O_RDWR);
	// closing STDIN, STDOUT and STDERR
	for (int x = 0; x < 3; ++x) {
		dup2(devnull, x); // cannot signalize about error
	}
	close(devnull);
}

int main()
{
	logging_init();	
	daemonize();
	logging(INFO, "daemon started");
	living = 1;
	while (living)
	{
		logging(INFO, "sleepin");
		//TODO: Insert daemon code here.
		sleep(1);
		//break;
    }

	return EXIT_SUCCESS;
}


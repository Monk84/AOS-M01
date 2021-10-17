#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
int main(int argc, char * argv[])
{
	if (argc < 3){
		printf("Usage: %s filename mode\n", argv[0]);
		return -1;
	}
	mode_t m; 
	if (!strcmp(argv[2], "r")){
		m = S_IREAD;		
	} else if (!strcmp(argv[2], "rw")){
		m = S_IREAD | S_IWRITE;
	} else if (!strcmp(argv[2], "rwx")){
		m = S_IRWXU;
	} else if (!strcmp(argv[2], "w")){
		m = S_IWRITE;
	} else if (!strcmp(argv[2], "wx")){
		m = S_IWRITE | S_IEXEC;
	} else if (!strcmp(argv[2], "x")){
		m = S_IEXEC;
	} else {
		printf("Invalid mode!\n");
	}

	char * fname = argv[1];

	int fd = open(fname, O_CREAT | O_WRONLY, m);
	printf("%d\n", fd);
	char * string = "line1 \nline2 \n last line";
	write(fd, string, strlen(string));
	close(fd);
	fd = open(fname, O_RDONLY);
	printf("%d\n", fd);
	char buf[100] = {0};
	struct stat file_stat;
	fstat(fd, &file_stat);
	read(fd, &buf[0], file_stat.st_size);
	printf("File %s contains: %s\n", fname, buf);
	close(fd);
	fd = open(fname, O_RDWR);
	printf("%d\n", fd);
	if (-1 == fd) {
		//printf("%d\n", errno);
		perror("Some mistake");
	} else
		close(fd);
	return 0;
}


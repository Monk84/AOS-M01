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
		printf("Invalid mode\n");
	}

	char * fname = argv[1];

	int fd = open(fname, O_CREAT | O_WRONLY, m);
	printf("First open returned %d\n", fd);
	char * string = "line1 \n line2 \n last line";
	ssize_t wr = write(fd, string, strlen(string));
	printf("Write returned %ld\n", wr);
	int res = close(fd);
	printf("First close returned %d\n", res);
	fd = open(fname, O_RDONLY);
	printf("Second open returned %d\n", fd);
	char buf[100] = {0};
	struct stat file_stat;
	res = fstat(fd, &file_stat);
	printf("First fstat returned %d\n", res);
	wr = read(fd, &buf[0], file_stat.st_size);
	printf("Read returned %ld\n", wr);
	printf("File %s contains: %s\n", fname, buf);
	res = close(fd);
	printf("Second close returned %d\n", res);
	fd = open(fname, O_RDWR);
	printf("Third open returned %d\n", fd);
	if (-1 == fd){
		//printf("%d\n", errno);
		perror("Some mistake");
	} else {
		res = close(fd);
		printf("Third close returned %d\n", res);
	}
	
	return 0;
}


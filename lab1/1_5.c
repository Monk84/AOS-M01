#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
int main(void)
{
	char * fname = "1_4.txt";
	char buf[100] = {0};
	sprintf(buf, "./2.exe %s rw", fname);
	system(buf);

	memset(buf, 0, sizeof(buf));
	int fd = open(fname, O_RDWR);
	printf("open returned %d\n", fd);
	//reading 3 characters starting from position 9
	int pos = lseek(fd, 9, SEEK_CUR);
	printf("lseek(fd, 9, SEEK_CUR) returned %d\n", pos);
	pos = read(fd, &buf[0], 3);
	printf("read(fd, ptr, 3) returned %d\n", pos);
	printf("1: read 3 chars from pos 9\n%s\n", buf);
	//writing at the end of file
	pos = lseek(fd, 0, SEEK_END);
	printf("lseek(fd, 0, SEEK_END) returned %d\n", pos);
	pos = write(fd, &buf[0], 3);

	//confirm changes - write full modified file
	pos = lseek(fd, 0 , SEEK_SET);
	struct stat st_buf;
	fstat(fd, &st_buf);
	pos = read(fd, &buf[0], st_buf.st_size);
	printf("read(fd, ptr, st_size) returned %d\n", pos);
	printf("2: wrote 3 chars into the end of file\n%s\n", buf);
	close(fd);
	(void)pos;
	return 0;
}


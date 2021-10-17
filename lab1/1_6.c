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
	//reading 3 characters starting from position 9
	int pos = lseek(fd, 9, SEEK_CUR);
	pos = read(fd, &buf[0], 3);
	printf("1: read 3 chars from pos 9\n%s\n", buf);
	//writing at the end of file
	pos = lseek(fd, 1024, SEEK_END);
	write(fd, &buf[0], 3);
	//confirm changes - write full modified file
	pos = lseek(fd, 0 , SEEK_SET);
	struct stat st_buf;
	fstat(fd, &st_buf);
	read(fd, &buf[0], st_buf.st_size);
	printf("2: wrote 3 chars into the end of file\n");
	for (int i = 0; i < st_buf.st_size; ++i)
		if (buf[i])
			printf("%c", buf[i]);
		else
			printf("\\0");
	printf("\n");
	close(fd);
	(void)pos;
	return 0;
}


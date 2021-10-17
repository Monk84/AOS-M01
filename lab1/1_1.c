#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

int main(void)
{
	int fd = open("1.txt", O_DIRECTORY);
	if (-1 == fd) {
		if (ENOENT == errno)
			printf("open returned ENOENT\n");
		printf("errno value: %d\n", errno);
		perror("open file 1.txt returned error");
	}
	printf("sys_err: %s", sys_errlist[sys_nerr - 1]);
	return 0;
}


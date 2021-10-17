#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s filename\n", argv[0]);
		exit(1);
	}
	//int fsiz = lseek(fd, 0, SEEK_END);
	//lseek(fd, 0, SEEK_SET);
	switch (fork()) {
	case -1:
		perror("fork");
		exit(1);
		break;
	case 0: //CHILD
	{	
		int fd = open(argv[1], O_RDONLY);
		char childb[200] = {0};
		int fchild = open("child8.txt", O_TRUNC | O_CREAT | O_WRONLY);
		int chread;
		while ((chread = read(fd, &childb[0], 6))) {
#if defined PRINT
			printf("CHILD: read %d symbs:\n%s\n", chread, childb);
#endif
			write(fchild, &childb[0], chread);
		}
		break;
	}
	default: //PARENT
	{
		int fd = open(argv[1], O_RDONLY);
		char parentb[200] = {0};
		int fparent = open("parent8.txt", O_TRUNC | O_CREAT | O_WRONLY);
		int parread;
		while ((parread = read(fd, &parentb[0], 6))) {
#if defined PRINT
			printf("PARENT: read %d symbs:\n%s\n", parread, parentb);
#endif
			write(fparent, &parentb[0], parread);
		}
		wait(NULL);
		break;
	}
	}
	return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/* 2.10
 * Создать при помощи fork - exec параллельный процесс, его имя 
 * передается как аргумент командной строки. Передать в порожденный 
 * процесс некоторую информацию через список параметров (список 
 * аргументов функции main). Каждый процесс должен вывести список 
 * переданных ему аргументов и свое окружение
 * */

extern char **environ;

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s args\n", argv[0]);
		exit(1);
	}
	pid_t forked;
	switch((forked = fork())) {
	case -1: //err
		perror("fork:");
		return 2;
		break;
	case 0: {
		// exec
		printf("\nCHILD: %d arguments to main(): ", argc);
		for (int i = 1; i < argc; ++i)
			printf(" arg %d: \"%s\"", i, argv[i]);
		printf("CHILD: environment args:\n");
		int envcnt = 0;
		for (; NULL != environ[envcnt]; ++envcnt);
		for (int i = envcnt / 2; i < envcnt; ++i)
			printf(environ[i]);
		printf("\n");
		execvp(argv[1], argv + 1);
		break;
	}
	default: {
		printf("\nPARENT: %d: ", argc);
		for (int i = 1; i < argc; ++i)
			printf(" arg %d: \"%s\"", i, argv[i]);
		printf("PARENT: environment args:\n");
		int envcnt = 0;
		for (; NULL != environ[envcnt]; ++envcnt);
		for (int i = envcnt / 2; i < envcnt; ++i)
			printf(environ[i]);
		printf("\n");
		break;
	}
	};
	return 0;
}
